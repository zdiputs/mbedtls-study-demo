/**
 * @brief   RSA Sign Function demo
 * @author  mculover666
 * @date    2020/10/03
*/

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_RSA_C)

#include <stdio.h>
#include "string.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/rsa.h"

static char buf[516];

static void dump_rsa_key(mbedtls_rsa_context *ctx)
{
    size_t olen;
    
    printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");
    mbedtls_mpi_write_string(&ctx->N , 16, buf, sizeof(buf), &olen);
    printf("N: %s\n", buf); 

    mbedtls_mpi_write_string(&ctx->E , 16, buf, sizeof(buf), &olen);
    printf("E: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->D , 16, buf, sizeof(buf), &olen);
    printf("D: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->P , 16, buf, sizeof(buf), &olen);
    printf("P: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->Q , 16, buf, sizeof(buf), &olen);
    printf("Q: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->DP, 16, buf, sizeof(buf), &olen);
    printf("DP: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->DQ, 16, buf, sizeof(buf), &olen);
    printf("DQ: %s\n", buf);

    mbedtls_mpi_write_string(&ctx->QP, 16, buf, sizeof(buf), &olen);
    printf("QP: %s\n", buf);
    printf("\n  +++++++++++++++++ rsa keypair +++++++++++++++++\n\n");
}

static void dump_buf(uint8_t *buf, uint32_t len)
{
    int i;
    
    for (i = 0; i < len; i++) {
        printf("%s%02X%s", i % 16 == 0 ? "\r\n\t" : " ", 
                           buf[i], 
                           i == len - 1 ? "\r\n" : "");
    }
}

static uint8_t output_buf[2048/8];

int mbedtls_rsa_sign_test(void)
{
    int ret;
    const char* msg = "HelloWorld";
   
    const char *pers = "rsa_sign_test";
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_rsa_context ctx;

    /* 1. init structure */
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V21, MBEDTLS_MD_SHA256);
    
    /* 2. update seed with we own interface ported */
    printf( "\n  . Seeding the random number generator..." );
    
    ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen(pers));
    if(ret != 0) {
        printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    printf( " ok\n" );

    /* 3. generate an RSA keypair */
    printf( "\n  . Generate RSA keypair..." );
    
    ret = mbedtls_rsa_gen_key(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg, 2048, 65537);
    if(ret != 0) {
        printf( " failed\n  ! mbedtls_rsa_gen_key returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    printf( " ok\n" );
    
    /* shwo RSA keypair */
    dump_rsa_key(&ctx);
    
    /* 4. sign */
    printf( "\n  . RSA pkcs1 sign..." );
    
    ret = mbedtls_rsa_pkcs1_sign(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256, strlen(msg), (uint8_t *)msg, output_buf);
    if(ret != 0) {
        printf( " failed\n  ! mbedtls_rsa_pkcs1_sign returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    printf( " ok\n" );
    
    /* show sign result */
    dump_buf(output_buf, sizeof(output_buf));
    
    /* 5. verify sign*/
    printf( "\n  . RSA pkcs1 verify..." );
    
    ret = mbedtls_rsa_pkcs1_verify(&ctx, mbedtls_ctr_drbg_random, &ctr_drbg, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, strlen(msg), (uint8_t *)msg, output_buf);
    
    if(ret != 0) {
        printf( " failed\n  ! mbedtls_rsa_pkcs1_encrypt returned %d(-0x%04x)\n", ret, -ret);
        goto exit;
    }
    printf( " ok\n" );
    
    exit:
    
    /* 5. release structure */
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_rsa_free(&ctx);
    
    return ret;
}

#endif /* MBEDTLS_RSA_C */
