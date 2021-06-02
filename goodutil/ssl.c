/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "ssl.h"

#ifdef HEADER_SSL_H

void good_ssl_ctx_freep(SSL_CTX **ctx)
{
    if (!ctx || !*ctx)
        return;

    SSL_CTX_free(*ctx);

    /*Set to NULL(0).*/
    *ctx = NULL;
}

static int _good_ssl_ctx_init(void *opaque)
{
    SSL_METHOD **method = (SSL_METHOD **)opaque;

    method[0] = (SSL_METHOD *)TLSv1_2_server_method();
    method[1] = (SSL_METHOD *)TLSv1_2_client_method();

    method[2] = (SSL_METHOD *)TLSv1_1_server_method();
    method[3] = (SSL_METHOD *)TLSv1_1_client_method();

    method[4] = (SSL_METHOD *)TLSv1_server_method();
    method[5] = (SSL_METHOD *)TLSv1_client_method();

    method[6] = (SSL_METHOD *)SSLv3_server_method();
    method[7] = (SSL_METHOD *)SSLv3_client_method();

    method[8] = (SSL_METHOD *)SSLv23_server_method();
    method[9] = (SSL_METHOD *)SSLv23_client_method();

    return 0;
}

SSL_CTX *good_ssl_ctx_alloc(int version, int server)
{
    static int init = 0;
    static SSL_METHOD *method[10] = {0};
    SSL_METHOD *p;

    assert(good_once(&init, _good_ssl_ctx_init, &method) >= 0);

    if (version == 12)
        p = server ? method[0] : method[1];
    else if (version == 11)
        p = server ? method[2] : method[3];
    else if (version == 10)
        p = server ? method[4] : method[5];
    else if (version == 30)
        p = server ? method[6] : method[7];
    else if (version == 23)
        p = server ? method[8] : method[9];

    return SSL_CTX_new(p);
}

int good_ssl_ctx_load_cert(SSL_CTX *ctx, const char *cert, const char *key, const char *pwd)
{
    int chk;

    assert(ctx != NULL);

    if (cert)
    {
        chk = SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM);
        if (chk != 1)
            GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);
    }

    if (key && pwd)
        SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)pwd);

    if (key)
    {
        chk = SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM);
        if (chk != 1)
            GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);
    }

    if (cert && key)
    {
        chk = SSL_CTX_check_private_key(ctx);
        if (chk != 1)
            GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);
    }

    return 0;

final_error:

    return -1;
}

void good_ssl_freep(SSL **ssl)
{
    if (!ssl || !*ssl)
        return;

    SSL_shutdown(*ssl);
    SSL_free(*ssl);

    /*Set to NULL(0).*/
    *ssl = NULL;
}

SSL *good_ssl_alloc(SSL_CTX *ctx)
{
    assert(ctx != NULL);

    return SSL_new(ctx);
}

int good_ssl_handshake(int fd, SSL *ssl, int server, time_t timeout)
{
    int err;
    int chk;

    assert(fd >= 0 && ssl != NULL);

    if (SSL_get_fd(ssl) != fd)
    {
        chk = SSL_set_fd(ssl, fd);
        if (chk != 1)
            GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);

        if (server)
            SSL_set_accept_state(ssl);
        else
            SSL_set_connect_state(ssl);
    }

try_again:

    chk = SSL_do_handshake(ssl);
    if (chk == 1)
        goto final;

    err = SSL_get_error(ssl, chk);
    if (err == SSL_ERROR_WANT_WRITE)
    {
        chk = good_poll(fd, 0x02, timeout);
        if (chk <= 0)
            GOOD_ERRNO_AND_GOTO1(ETIMEDOUT, final_error);

        GOOD_ERRNO_AND_GOTO1(0, try_again);
    }
    else if (err == SSL_ERROR_WANT_READ)
    {
        chk = good_poll(fd, 0x01, timeout);
        if (chk <= 0)
            GOOD_ERRNO_AND_GOTO1(ETIMEDOUT, final_error);

        GOOD_ERRNO_AND_GOTO1(0, try_again);
    }
    else
    {
        GOOD_ERRNO_AND_GOTO1(ENOSYS, final_error);
    }

final:

    return 0;

final_error:

    return -1;
}

#endif //HEADER_SSL_H