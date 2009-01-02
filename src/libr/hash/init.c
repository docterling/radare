/* This file is part of radare libr.
 * It is licensed under the LGPL license
 */

#include "r_hash.h"
#include "md5.h"
#include "sha1.h"
#include "sha2.h"

// TODO: to be moved to r_hash_internal.h
void mdfour(u8 *out, const u8 *in, int n);

#define CHKFLAG(f,x) if (f==0||f&x)

void r_hash_init(struct r_hash_t *ctx, int flags)
{
	CHKFLAG(flags,R_HASH_MD5)    MD5Init(&ctx->md5);
	CHKFLAG(flags,R_HASH_SHA1)   SHA1_Init(&ctx->sha1);
	CHKFLAG(flags,R_HASH_SHA256) SHA256_Init(&ctx->sha256);
	CHKFLAG(flags,R_HASH_SHA384) SHA384_Init(&ctx->sha384);
	CHKFLAG(flags,R_HASH_SHA512) SHA512_Init(&ctx->sha512);
}

const u8 *r_hash_md5(struct r_hash_t *ctx, const u8 *input, u32 len)
{
	MD5Update(&ctx->md5, input, len);
	MD5Final(&ctx->digest, &ctx->md5);
	return ctx->digest;
}

const u8 *r_hash_sha1(struct r_hash_t *ctx, const u8 *input, u32 len)
{
	SHA1_Update(&ctx->sha1, input, len);
	SHA1_Final(ctx->digest, &ctx->sha1);
	return ctx->digest;
}

const u8 *r_hash_md4(struct r_hash_t *ctx, const u8 *input, u32 len)
{
	mdfour(ctx->digest, input, len);
	return ctx->digest;
}

const u8 *r_hash_sha256(struct r_hash_t *ctx, const u8 *input, u32 len)
{
	SHA256_Update(&ctx->sha256, input, len);
	SHA256_Final(ctx->digest, &ctx->sha256);
	return ctx->digest;
}

const u8 *r_hash_sha384(struct r_hash_t *ctx, const u8 *input, u32 len)
{
	SHA384_Update(&ctx->sha384, input, len);
	SHA384_Final(ctx->digest, &ctx->sha384);
	return ctx->digest;
}

const u8 *r_hash_sha512(struct r_hash_t *ctx, const u8 *input, u32 len)
{
	SHA512_Update(&ctx->sha512, input, len);
	SHA512_Final(ctx->digest, &ctx->sha512);
	return ctx->digest;
}