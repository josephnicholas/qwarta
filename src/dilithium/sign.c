#include <stdint.h>
#include "params.h"
#include "sign.h"
#include "randombytes.h"
#include "../crypto/random.h"
#include "fips202.h"
#include "fips202x4.h"
#include "poly.h"
#include "polyvec.h"
#include "packing.h"

/*************************************************
* Name:        expand_mat
*
* Description: Implementation of ExpandA. Generates matrix A with uniformly
*              random coefficients a_{i,j} by performing rejection
*              sampling on the output stream of SHAKE128(rho|i|j).
*
* Arguments:   - polyvecl mat[K]: output matrix
*              - const unsigned char rho[]: byte array containing seed rho
**************************************************/
void expand_mat(polyvecl mat[_K], const unsigned char rho[SEEDBYTES]) {
  unsigned int i, j;
  unsigned char inbuf[SEEDBYTES + 1];
  /* Don't change this to smaller values,
   * sampling later assumes sufficient SHAKE output!
   * Probability that we need more than 5 blocks: < 2^{-132}.
   * Probability that we need more than 6 blocks: < 2^{-546}. */
  unsigned char outbuf[5*SHAKE128_RATE];

  for(i = 0; i < SEEDBYTES; ++i)
    inbuf[i] = rho[i];

  for(i = 0; i < _K; ++i) {
    for(j = 0; j < _L; ++j) {
      inbuf[SEEDBYTES] = i + (j << 4);
      shake128(outbuf, sizeof(outbuf), inbuf, SEEDBYTES + 1);
      poly_uniform(mat[i].vec+j, outbuf);
    }
  }
}

#if _L == 2 && _K == 3

void expand_mat_avx(polyvecl mat[3], const unsigned char rho[SEEDBYTES])
{
  unsigned int i;
  unsigned char inbuf[4][SEEDBYTES + 1];
  unsigned char outbuf[4][5*SHAKE128_RATE];

  for(i = 0; i < SEEDBYTES; ++i) {
    inbuf[0][i] = rho[i];
    inbuf[1][i] = rho[i];
    inbuf[2][i] = rho[i];
    inbuf[3][i] = rho[i];
  }

  inbuf[0][SEEDBYTES] = 0 + (0 << 4);
  inbuf[1][SEEDBYTES] = 0 + (1 << 4);
  inbuf[2][SEEDBYTES] = 1 + (0 << 4);
  inbuf[3][SEEDBYTES] = 1 + (1 << 4);

  shake128_4x(outbuf[0], outbuf[1], outbuf[2], outbuf[3], 5*SHAKE128_RATE,
              inbuf[0], inbuf[1], inbuf[2], inbuf[3], SEEDBYTES + 1);

  poly_uniform(&mat[0].vec[0], outbuf[0]);
  poly_uniform(&mat[0].vec[1], outbuf[1]);
  poly_uniform(&mat[1].vec[0], outbuf[2]);
  poly_uniform(&mat[1].vec[1], outbuf[3]);

  inbuf[0][SEEDBYTES] = 2 + (0 << 4);
  inbuf[1][SEEDBYTES] = 2 + (1 << 4);

  shake128_4x(outbuf[0], outbuf[1], outbuf[2], outbuf[3], 5*SHAKE128_RATE,
              inbuf[0], inbuf[1], inbuf[2], inbuf[3], SEEDBYTES + 1);

  poly_uniform(&mat[2].vec[0], outbuf[0]);
  poly_uniform(&mat[2].vec[1], outbuf[1]);
}

#elif _L == 3 && _K == 4

void expand_mat_avx(polyvecl mat[4], const unsigned char rho[SEEDBYTES])
{
  unsigned int i;
  unsigned char inbuf[4][SEEDBYTES + 1];
  unsigned char outbuf[4][5*SHAKE128_RATE];

  for(i = 0; i < SEEDBYTES; ++i) {
    inbuf[0][i] = rho[i];
    inbuf[1][i] = rho[i];
    inbuf[2][i] = rho[i];
    inbuf[3][i] = rho[i];
  }

  for(i = 0; i < 3; ++i) {
    inbuf[0][SEEDBYTES] = 0 + (i << 4);
    inbuf[1][SEEDBYTES] = 1 + (i << 4);
    inbuf[2][SEEDBYTES] = 2 + (i << 4);
    inbuf[3][SEEDBYTES] = 3 + (i << 4);

    shake128_4x(outbuf[0], outbuf[1], outbuf[2], outbuf[3], 5*SHAKE128_RATE,
                inbuf[0], inbuf[1], inbuf[2], inbuf[3], SEEDBYTES + 1);

    poly_uniform(&mat[0].vec[i], outbuf[0]);
    poly_uniform(&mat[1].vec[i], outbuf[1]);
    poly_uniform(&mat[2].vec[i], outbuf[2]);
    poly_uniform(&mat[3].vec[i], outbuf[3]);
  }
}

#elif _L == 4 && _K == 5

void expand_mat_avx(polyvecl mat[5], const unsigned char rho[SEEDBYTES])
{
  unsigned int i;
  unsigned char inbuf[4][SEEDBYTES + 1];
  unsigned char outbuf[4][5*SHAKE128_RATE];

  for(i = 0; i < SEEDBYTES; ++i) {
    inbuf[0][i] = rho[i];
    inbuf[1][i] = rho[i];
    inbuf[2][i] = rho[i];
    inbuf[3][i] = rho[i];
  }

  for(i = 0; i < 5; ++i) {
    inbuf[0][SEEDBYTES] = i + (0 << 4);
    inbuf[1][SEEDBYTES] = i + (1 << 4);
    inbuf[2][SEEDBYTES] = i + (2 << 4);
    inbuf[3][SEEDBYTES] = i + (3 << 4);

    shake128_4x(outbuf[0], outbuf[1], outbuf[2], outbuf[3], 5*SHAKE128_RATE,
                inbuf[0], inbuf[1], inbuf[2], inbuf[3], SEEDBYTES + 1);

    poly_uniform(&mat[i].vec[0], outbuf[0]);
    poly_uniform(&mat[i].vec[1], outbuf[1]);
    poly_uniform(&mat[i].vec[2], outbuf[2]);
    poly_uniform(&mat[i].vec[3], outbuf[3]);
  }
}

#elif _L == 5 && _K == 6

void expand_mat_avx(polyvecl mat[6], const unsigned char rho[SEEDBYTES])
{
  unsigned int i;
  unsigned char inbuf[4][SEEDBYTES + 1];
  unsigned char outbuf[4][5*SHAKE128_RATE];

  for(i = 0; i < SEEDBYTES; ++i) {
    inbuf[0][i] = rho[i];
    inbuf[1][i] = rho[i];
    inbuf[2][i] = rho[i];
    inbuf[3][i] = rho[i];
  }

  for(i = 0; i < 6; ++i) {
    inbuf[0][SEEDBYTES] = i + (0 << 4);
    inbuf[1][SEEDBYTES] = i + (1 << 4);
    inbuf[2][SEEDBYTES] = i + (2 << 4);
    inbuf[3][SEEDBYTES] = i + (3 << 4);

    shake128_4x(outbuf[0], outbuf[1], outbuf[2], outbuf[3], 5*SHAKE128_RATE,
                inbuf[0], inbuf[1], inbuf[2], inbuf[3], SEEDBYTES + 1);

    poly_uniform(&mat[i].vec[0], outbuf[0]);
    poly_uniform(&mat[i].vec[1], outbuf[1]);
    poly_uniform(&mat[i].vec[2], outbuf[2]);
    poly_uniform(&mat[i].vec[3], outbuf[3]);
  }

  inbuf[0][SEEDBYTES] = 0 + (4 << 4);
  inbuf[1][SEEDBYTES] = 1 + (4 << 4);
  inbuf[2][SEEDBYTES] = 2 + (4 << 4);
  inbuf[3][SEEDBYTES] = 3 + (4 << 4);

  shake128_4x(outbuf[0], outbuf[1], outbuf[2], outbuf[3], 5*SHAKE128_RATE,
              inbuf[0], inbuf[1], inbuf[2], inbuf[3], SEEDBYTES + 1);

  poly_uniform(&mat[0].vec[4], outbuf[0]);
  poly_uniform(&mat[1].vec[4], outbuf[1]);
  poly_uniform(&mat[2].vec[4], outbuf[2]);
  poly_uniform(&mat[3].vec[4], outbuf[3]);

  inbuf[0][SEEDBYTES] = 4 + (4 << 4);
  inbuf[1][SEEDBYTES] = 5 + (4 << 4);

  shake128_4x(outbuf[0], outbuf[1], outbuf[2], outbuf[3], 5*SHAKE128_RATE,
              inbuf[0], inbuf[1], inbuf[2], inbuf[3], SEEDBYTES + 1);

  poly_uniform(&mat[4].vec[4], outbuf[0]);
  poly_uniform(&mat[5].vec[4], outbuf[1]);
}

#else
#error
#endif

/*************************************************
* Name:        challenge
*
* Description: Implementation of H. Samples polynomial with 60 nonzero
*              coefficients in {-1,1} using the output stream of
*              SHAKE256(mu|w1).
*
* Arguments:   - poly *c: pointer to output polynomial
*              - const unsigned char mu[]: byte array containing mu
*              - const polyveck *w1: pointer to vector w1
**************************************************/
void challenge(poly *c,
               const unsigned char mu[CRHBYTES],
               const polyveck *w1)
{
  unsigned int i, b, pos;
  unsigned char inbuf[CRHBYTES + _K*POLW1_SIZE_PACKED];
  unsigned char outbuf[SHAKE256_RATE];
  uint64_t state[25], signs, mask;

  for(i = 0; i < CRHBYTES; ++i)
    inbuf[i] = mu[i];
  for(i = 0; i < _K; ++i)
    polyw1_pack(inbuf + CRHBYTES + i*POLW1_SIZE_PACKED, w1->vec+i);

  shake256_absorb(state, inbuf, sizeof(inbuf));
  shake256_squeezeblocks(outbuf, 1, state);

  signs = 0;
  for(i = 0; i < 8; ++i)
    signs |= (uint64_t)outbuf[i] << 8*i;

  pos = 8;
  mask = 1;

  for(i = 0; i < _N; ++i)
    c->coeffs[i] = 0;

  for(i = 196; i < 256; ++i) {
    do {
      if(pos >= SHAKE256_RATE) {
        shake256_squeezeblocks(outbuf, 1, state);
        pos = 0;
      }

      b = outbuf[pos++];
    } while(b > i);

    c->coeffs[i] = c->coeffs[b];
    c->coeffs[b] = (signs & mask) ? _Q - 1 : 1;
    mask <<= 1;
  }
}

/*************************************************
* Name:        crypto_sign_keypair
*
* Description: Generates public and private key.
*
* Arguments:   - unsigned char *pk: pointer to output public key (allocated
*                                   array of CRYPTO_PUBLICKEYBYTES bytes)
*              - unsigned char *sk: pointer to output private key (allocated
*                                   array of CRYPTO_SECRETKEYBYTES bytes)
*
* Returns 0 (success)
**************************************************/
int crypto_sign_keypair(unsigned char *pk, unsigned char *sk) {
  unsigned int i;
  unsigned char seedbuf[3*SEEDBYTES];
  unsigned char tr[CRHBYTES];
  unsigned char *rho, *rhoprime, *key;
  uint16_t nonce = 0;
  polyvecl mat[_K];
  polyvecl s1, s1hat;
  polyveck s2, t, t1, t0;

  /* Expand 32 bytes of randomness into rho, rhoprime and key */
  randombytes(seedbuf, SEEDBYTES);
  shake256(seedbuf, 3*SEEDBYTES, seedbuf, SEEDBYTES);
  rho = seedbuf;
  rhoprime = rho + SEEDBYTES;
  key = rho + 2*SEEDBYTES;

  /* Expand matrix */
  expand_mat_avx(mat, rho);

  /* Sample short vectors s1 and s2 */
#if _L == 2 && _K == 3
  poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s2.vec[0], &s2.vec[1], rhoprime,
                      nonce, nonce + 1, nonce + 2, nonce + 3);
  poly_uniform_eta(&s2.vec[2], rhoprime, nonce + 4);
  nonce += 5;
#elif _L == 3 && _K == 4
  poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s2.vec[0], rhoprime,
                      nonce, nonce + 1, nonce + 2, nonce + 3);
  poly_uniform_eta_4x(&s2.vec[1], &s2.vec[2], &s2.vec[3], &t.vec[0], rhoprime,
                      nonce + 4, nonce + 5, nonce + 6, nonce + 7);
  nonce += 7;
#elif _L == 4 && _K == 5
  poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s1.vec[3], rhoprime,
                      nonce, nonce + 1, nonce + 2, nonce + 3);
  poly_uniform_eta_4x(&s2.vec[0], &s2.vec[1], &s2.vec[2], &s2.vec[3], rhoprime,
                      nonce + 4, nonce + 5, nonce + 6, nonce + 7);
  poly_uniform_eta(&s2.vec[4], rhoprime, nonce + 8);
  nonce += 9;
#elif _L == 5 && _K == 6
  poly_uniform_eta_4x(&s1.vec[0], &s1.vec[1], &s1.vec[2], &s1.vec[3], rhoprime,
                      nonce, nonce + 1, nonce + 2, nonce + 3);
  poly_uniform_eta_4x(&s1.vec[4], &s2.vec[0], &s2.vec[1], &s2.vec[2], rhoprime,
                      nonce + 4, nonce + 5, nonce + 6, nonce + 7);
  poly_uniform_eta_4x(&s2.vec[3], &s2.vec[4], &s2.vec[5], &t.vec[0], rhoprime,
                      nonce + 8, nonce + 9, nonce + 10, nonce + 11);
  nonce += 11;
#else
#error ""
#endif

  /* Matrix-vector multiplication */
  s1hat = s1;
  polyvecl_ntt(&s1hat);
  for(i = 0; i < _K; ++i) {
    polyvecl_pointwise_acc_invmontgomery(t.vec+i, mat+i, &s1hat);
    poly_reduce(t.vec+i);
    poly_invntt_montgomery(t.vec+i);
  }

  /* Add noise vector s2 */
  polyveck_add(&t, &t, &s2);

  /* Extract t1 and write public key */
  polyveck_freeze(&t);
  polyveck_power2round(&t1, &t0, &t);
  pack_pk(pk, rho, &t1);

  /* Compute CRH(rho, t1) and write secret key */
  shake256(tr, CRHBYTES, pk, CRYPTO_PUBLICKEYBYTES);
  pack_sk(sk, rho, key, tr, &s1, &s2, &t0);

  return 0;
}

/*************************************************
* Name:        crypto_sign
*
* Description: Compute signed message.
*
* Arguments:   - unsigned char *sm: pointer to output signed message (allocated
*                                   array with CRYPTO_BYTES + mlen bytes),
*                                   can be equal to m
*              - unsigned long long *smlen: pointer to output length of signed
*                                           message
*              - const unsigned char *m: pointer to message to be signed
*              - unsigned long long mlen: length of message
*              - const unsigned char *sk: pointer to bit-packed secret key
*
* Returns 0 (success)
**************************************************/
int crypto_sign(unsigned char *sm,
                unsigned long long *smlen,
                const unsigned char *m,
                unsigned long long mlen,
                const unsigned char *sk)
{
  unsigned long long i;
  unsigned int n;
  unsigned char seedbuf[2*SEEDBYTES + CRHBYTES]; // TODO: nonce in seedbuf (2x)
  unsigned char tr[CRHBYTES];
  unsigned char *rho, *key, *mu;
  uint16_t nonce = 0;
  poly c, chat;
  polyvecl mat[_K], s1, y, yhat, z;
  polyveck t0, s2, w, w1, w0;
  polyveck h, cs2, ct0;

  rho = seedbuf;
  key = seedbuf + SEEDBYTES;
  mu = seedbuf + 2*SEEDBYTES;
  unpack_sk(rho, key, tr, &s1, &s2, &t0, sk);

  /* Copy tr and message into the sm buffer,
   * backwards since m and sm can be equal in SUPERCOP API */
  for(i = 1; i <= mlen; ++i)
    sm[CRYPTO_BYTES + mlen - i] = m[mlen - i];
  for(i = 0; i < CRHBYTES; ++i)
    sm[CRYPTO_BYTES - CRHBYTES + i] = tr[i];

  /* Compute CRH(tr, msg) */
  shake256(mu, CRHBYTES, sm + CRYPTO_BYTES - CRHBYTES, CRHBYTES + mlen);

  /* Expand matrix and transform vectors */
  expand_mat_avx(mat, rho);
  polyvecl_ntt(&s1);
  polyveck_ntt(&s2);
  polyveck_ntt(&t0);

  rej:
  /* Sample intermediate vector y */
#if _L == 2
  poly_uniform_gamma1m1_4x(&y.vec[0], &y.vec[1], &yhat.vec[0], &yhat.vec[1], key,
                           nonce, nonce + 1, nonce + 2, nonce + 3);
  nonce += 2;
#elif _L == 3
  poly_uniform_gamma1m1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &yhat.vec[0], key,
                           nonce, nonce + 1, nonce + 2, nonce + 3);
  nonce += 3;
#elif _L == 4
  poly_uniform_gamma1m1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &y.vec[3], key,
                           nonce, nonce + 1, nonce + 2, nonce + 3);
  nonce += 4;
#elif _L == 5
  poly_uniform_gamma1m1_4x(&y.vec[0], &y.vec[1], &y.vec[2], &y.vec[3], key,
                           nonce, nonce + 1, nonce + 2, nonce + 3);
  poly_uniform_gamma1m1(&y.vec[4], key, nonce + 4);
  nonce += 5;
#else
#error
#endif

  /* Matrix-vector multiplication */
  yhat = y;
  polyvecl_ntt(&yhat);
  for(i = 0; i < _K; ++i) {
    polyvecl_pointwise_acc_invmontgomery(w.vec+i, mat+i, &yhat);
    poly_reduce(w.vec+i);
    poly_invntt_montgomery(w.vec+i);
  }

  /* Decompose w and call random oracle */
  polyveck_csubq(&w);
  polyveck_decompose(&w1, &w0, &w);
  challenge(&c, mu, &w1);
  chat = c;
  poly_ntt(&chat);

  /* Check that subtracting cs2 does not change high bits of w and low bits
   * do not reveal secret information */
  for(i = 0; i < _K; ++i) {
    poly_pointwise_invmontgomery(cs2.vec+i, &chat, s2.vec+i);
    poly_invntt_montgomery(cs2.vec+i);
  }
  polyveck_sub(&w0, &w0, &cs2);
  polyveck_freeze(&w0);
  if(polyveck_chknorm(&w0, GAMMA2 - BETA))
    goto rej;

  /* Compute z, reject if it reveals secret */
  for(i = 0; i < _L; ++i) {
    poly_pointwise_invmontgomery(z.vec+i, &chat, s1.vec+i);
    poly_invntt_montgomery(z.vec+i);
  }
  polyvecl_add(&z, &z, &y);
  polyvecl_freeze(&z);
  if(polyvecl_chknorm(&z, GAMMA1 - BETA))
    goto rej;

  /* Compute hints for w1 */
  for(i = 0; i < _K; ++i) {
    poly_pointwise_invmontgomery(ct0.vec+i, &chat, t0.vec+i);
    poly_invntt_montgomery(ct0.vec+i);
  }

  polyveck_csubq(&ct0);
  if(polyveck_chknorm(&ct0, GAMMA2))
    goto rej;

  polyveck_add(&w0, &w0, &ct0);
  polyveck_csubq(&w0);
  n = polyveck_make_hint(&h, &w0, &w1);
  if(n > OMEGA)
    goto rej;

  /* Write signature */
  pack_sig(sm, &z, &h, &c);

  *smlen = mlen + CRYPTO_BYTES;
  return 0;
}

/*************************************************
* Name:        crypto_sign_open
*
* Description: Verify signed message.
*
* Arguments:   - unsigned char *m: pointer to output message (allocated
*                                  array with smlen bytes), can be equal to sm
*              - unsigned long long *mlen: pointer to output length of message
*              - const unsigned char *sm: pointer to signed message
*              - unsigned long long smlen: length of signed message
*              - const unsigned char *sk: pointer to bit-packed public key
*
* Returns 0 if signed message could be verified correctly and -1 otherwise
**************************************************/
int crypto_sign_open(unsigned char *m,
                     unsigned long long *mlen,
                     const unsigned char *sm,
                     unsigned long long smlen,
                     const unsigned char *pk)
{
  unsigned long long i;
  unsigned char rho[SEEDBYTES];
  unsigned char mu[CRHBYTES];
  poly c, chat, cp;
  polyvecl mat[_K], z;
  polyveck t1, w1, h, tmp1, tmp2;

  if(smlen < CRYPTO_BYTES)
    goto badsig;

  *mlen = smlen - CRYPTO_BYTES;

  unpack_pk(rho, &t1, pk);
  if(unpack_sig(&z, &h, &c, sm))
    goto badsig;
  if(polyvecl_chknorm(&z, GAMMA1 - BETA))
    goto badsig;

  /* Compute CRH(CRH(rho, t1), msg) using m as "playground" buffer */
  if(sm != m)
    for(i = 0; i < *mlen; ++i)
      m[CRYPTO_BYTES + i] = sm[CRYPTO_BYTES + i];

  shake256(m + CRYPTO_BYTES - CRHBYTES, CRHBYTES, pk, CRYPTO_PUBLICKEYBYTES);
  shake256(mu, CRHBYTES, m + CRYPTO_BYTES - CRHBYTES, CRHBYTES + *mlen);

  /* Expand Matrix and transform vectors */
  expand_mat_avx(mat, rho);
  chat = c;
  poly_ntt(&chat);
  polyveck_shiftl(&t1, _D);
  polyveck_ntt(&t1);
  polyvecl_ntt(&z);

  /* Matrix-vector multiplication; compute Az - c2^dt1 */
  for(i = 0; i < _K ; ++i)
    polyvecl_pointwise_acc_invmontgomery(tmp1.vec+i, mat+i, &z);
  for(i = 0; i < _K; ++i)
    poly_pointwise_invmontgomery(tmp2.vec+i, &chat, t1.vec+i);
  polyveck_sub(&tmp1, &tmp1, &tmp2);
  polyveck_reduce(&tmp1);
  polyveck_invntt_montgomery(&tmp1);

  /* Reconstruct w1 */
  polyveck_csubq(&tmp1);
  polyveck_use_hint(&w1, &tmp1, &h);

  /* Call random oracle and verify challenge */
  challenge(&cp, mu, &w1);
  for(i = 0; i < _N; ++i)
    if(c.coeffs[i] != cp.coeffs[i])
      goto badsig;

  /* All good, copy msg, return 0 */
  for(i = 0; i < *mlen; ++i)
    m[i] = sm[CRYPTO_BYTES + i];

  return 0;

  /* Signature verification failed */
  badsig:
  *mlen = (unsigned long long) -1;
  for(i = 0; i < smlen; ++i)
    m[i] = 0;

  return -1;
}
