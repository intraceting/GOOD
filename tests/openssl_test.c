/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include "goodutil/getargs.h"
#include "goodutil/rsa.h"

#ifdef HEADER_RSA_H

void test_rsa(good_tree_t *opt)
{
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    ERR_load_crypto_strings();
    ERR_load_RSA_strings();

    RSA *key = NULL;
    RSA *prikey = NULL;
    RSA *pubkey = NULL;
    int chk;
#if 1
    prikey = good_rsa_from_file(
        good_option_get(opt, "--rsa-key-prifile", 0, ""),
        1,
        good_option_get(opt, "--rsa-key-pwd", 0, ""));
    pubkey = good_rsa_from_file(
        good_option_get(opt, "--rsa-key-pubfile", 0, ""),
        0,
        //good_option_get(opt, "--rsa-key-pwd", 0, ""));
        NULL);

    assert(prikey && pubkey);
#else

        key = good_rsa_create(2048,RSA_F4);

        int chk = good_rsa_to_file(
            good_option_get(opt, "--rsa-key-prifile", 0, ""),
            key,
            1,
            good_option_get(opt, "--rsa-key-pwd", 0, ""));

        assert(chk > 0);

        chk = good_rsa_to_file(
            good_option_get(opt, "--rsa-key-pubfile", 0, ""),
            key,
            0,
            good_option_get(opt, "--rsa-key-pwd", 0, ""));

        assert(chk > 0);

#endif 

    char *buf1 = (char*) good_heap_alloc(1000);
    char *buf2 = (char*) good_heap_alloc(2000);
    char *buf3 = (char*) good_heap_alloc(2000);

    memset(buf1,'a',1000);

    ssize_t m = good_rsa_ecb_encrypt(buf2,buf1,1000,pubkey,0,RSA_NO_PADDING);

    int e = ERR_get_error();
    char szErrMsg[1024] = {0};
    char *pTmp = NULL;
    pTmp = ERR_error_string(e,szErrMsg); 

    printf("%s\n",pTmp);

    assert(m>0);

    chk = good_rsa_ecb_decrypt(buf3,buf2,m,prikey,1,RSA_NO_PADDING);

    assert(chk >0);



    assert(memcmp(buf1,buf3,1000)==0);


    good_heap_free(buf1);
    good_heap_free(buf2);
    good_heap_free(buf3);


    RSA_free(key);
    RSA_free(prikey);
    RSA_free(pubkey);
}

#endif //HEADER_RSA_H


int main(int argc, char **argv)
{

    good_tree_t *t = good_tree_alloc(NULL);

    good_getargs(t,argc,argv,"--");

#ifdef HEADER_RSA_H

    test_rsa(t);
    
#endif //HEADER_RSA_H


    good_tree_free(&t);

    return 0;
}