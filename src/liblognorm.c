/* This file implements the liblognorm API.
 * See header file for descriptions.
 *
 * liblognorm - a fast samples-based log normalization library
 * Copyright 2010 by Rainer Gerhards and Adiscon GmbH.
 *
 * This file is part of liblognorm.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * A copy of the LGPL v2.1 can be found in the file "COPYING" in this distribution.
 */
#include "config.h"

#include "liblognorm.h"
#include "lognorm.h"
#include "samp.h"

#define ERR_ABORT {r = 1; goto done; }

#define CHECK_CTX \
	if(ctx->objID != LN_ObjID_CTX) { \
		r = -1; \
		goto done; \
	}

char *
ln_version(void)
{
	return VERSION;
}


ln_ctx
ln_initCtx(void)
{
	ln_ctx ctx;
	if((ctx = calloc(1, sizeof(struct ln_ctx_s))) == NULL)
		goto done;

	ctx->objID = LN_ObjID_CTX;
	ctx->dbgCB = NULL;

	/* we add an root for the empty word, this simplifies parse
	 * tree handling.
	 */
	if((ctx->ptree = ln_newPTree(ctx, NULL)) == NULL) {
		free(ctx);
		ctx = NULL;
		goto done;
	}

done:
	return ctx;
}


int
ln_exitCtx(ln_ctx ctx)
{
	int r = 0;

	CHECK_CTX;

	ctx->objID = LN_ObjID_None; /* prevent double free */
	if(ctx->ptree != NULL)
		ln_deletePTree(ctx->ptree);
	if(ctx->rulePrefix != NULL)
		es_deleteStr(ctx->rulePrefix);
	free(ctx);
done:
	return r;
}


int
ln_setDebugCB(ln_ctx ctx, void (*cb)(void*, char*, size_t), void *cookie)
{
	int r = 0;

	CHECK_CTX;
	ctx->dbgCB = cb;
	ctx->dbgCookie = cookie;
done:
	return r;
}


int
ln_loadSamples(ln_ctx ctx, char *file)
{
	int r = 0;
	struct ln_sampRepos *repo;
	struct ln_samp *samp;
	int isEof = 0;

	CHECK_CTX;
	if(file == NULL) ERR_ABORT;
	if((repo = ln_sampOpen(ctx, file)) == NULL) ERR_ABORT;
	while(!isEof) {
		if((samp = ln_sampRead(ctx, repo, &isEof)) == NULL) {
			/* TODO: what exactly to do? */
		}
	}
	ln_sampClose(ctx, repo);

done:
	return r;
}


void
ln_setEECtx(ln_ctx ctx, ee_ctx eectx)
{
	ctx->eectx = eectx;
}