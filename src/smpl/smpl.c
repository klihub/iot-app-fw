/*
 * Copyright (c) 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Intel Corporation nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <smpl/macros.h>
#include <smpl/types.h>
#include <smpl/smpl.h>


smpl_t *smpl_create(char ***errbuf)
{
    smpl_t *smpl;

    builtin_register();

    if (errbuf != NULL)
        *errbuf = NULL;

    smpl = smpl_alloct(typeof(*smpl));

    if (smpl == NULL)
        goto nomem;

    if (symtbl_create(smpl) < 0)
        goto nomem;

    smpl_list_init(&smpl->macros);
    smpl_list_init(&smpl->functions);
    smpl_list_init(&smpl->body);

    smpl->data   = symtbl_add(smpl, "data", SMPL_SYMBOL_DATA);
    smpl->errors = errbuf;

    if (smpl->data < 0)
        goto nodata;

    return smpl;

 nomem:
    smpl_free(smpl);
    return NULL;

 nodata:
    symtbl_destroy(smpl);
    smpl_free(smpl);
    return NULL;
}


void smpl_destroy(smpl_t *smpl)
{
    if (smpl == NULL)
        return;

    macro_purge(&smpl->macros);
    function_purge(&smpl->functions);
    block_free(&smpl->body);

    buffer_destroy(smpl->result);
    parser_destroy(smpl);
    symtbl_destroy(smpl);

    if (smpl->errors != NULL)
        smpl_errors_free(*smpl->errors);

    smpl_free(smpl);
}


smpl_t *smpl_load_template(const char *path, char ***errors)
{
    smpl_t *smpl;

    smpl = smpl_create(errors);

    if (smpl == NULL)
        goto nomem;

    smpl->result = buffer_create(8192);

    if (smpl->result == NULL)
        goto nomem;

    smpl->parser = parser_create(smpl);

    if (smpl->parser == NULL)
        goto nomem;

    if (preproc_push_file(smpl, path) < 0)
        goto file_fail;

    if (block_parse(smpl, SMPL_PARSE_MAIN, &smpl->body, NULL) != SMPL_TOKEN_EOF)
        goto parse_fail;

    preproc_trim(smpl);

    smpl->errors = NULL;

    return smpl;

 nomem:
    smpl_destroy(smpl);
    return NULL;

 file_fail:
    smpl_errmsg(smpl, errno, path, 0, "Failed to open template '%s'.", path);
    smpl->errors = NULL;
    smpl_destroy(smpl);

    return NULL;

 parse_fail:
    smpl_errmsg(smpl, errno, path, 0, "Failed to parse template.");
    smpl->errors = NULL;
    smpl_destroy(smpl);

    return NULL;

}


void smpl_free_template(smpl_t *smpl)
{
    if (smpl == NULL)
        return;

    smpl_destroy(smpl);
}


smpl_data_t *smpl_load_data(const char *path, char ***errors)
{
    return smpl_json_load(path, errors);
}


void smpl_free_data(smpl_data_t *data)
{
    smpl_json_free(data);
}


int smpl_add_function(smpl_t *smpl, char *name, smpl_fn_t fn, void *user_data)
{
    return function_register(smpl, name, fn, user_data);
}


int smpl_register_function(char *name, smpl_fn_t fn, void *user_data)
{
    return function_register(NULL, name, fn, user_data);
}


int smpl_unregister_function(char *name, smpl_fn_t fn)
{
    return function_unregister(NULL, name, fn);
}


smpl_value_t *smpl_value_set(smpl_value_t *v, int type, ...)
{
    va_list ap;

    va_start(ap, type);
    v = value_setv(v, type, ap);
    va_end(ap);

    return v;
}


int smpl_printf(smpl_t *smpl, const char *fmt, ...)
{
    va_list ap;
    int     r;

    if (smpl->callbacks <= 0)
        goto no_callback;

    va_start(ap, fmt);
    r = buffer_vprintf(smpl->result, fmt, ap);
    va_end(ap);

    return r;

 no_callback:
    smpl_fail(-1, smpl, EINVAL, "not in a function callback");
}


char *smpl_evaluate(smpl_t *smpl, smpl_data_t *data, char ***errors,
                    void *user_data)
{
    char *result;

    if (errors != NULL)
        *errors = NULL;

    smpl->errors    = errors;
    smpl->user_data = user_data;

    if (symtbl_push(smpl, smpl->data, SMPL_VALUE_OBJECT, data) < 0)
        goto data_fail;

    if (block_eval(smpl, &smpl->body, NULL) < 0)
        goto eval_fail;

    symtbl_flush(smpl);
    smpl->errors = NULL;

    result = buffer_steal(smpl->result);

    return result;

 data_fail:
    smpl_errmsg(smpl, errno, NULL, 0, "Failed to set substitution data.");
    smpl->errors = NULL;
    return NULL;

 eval_fail:
    smpl_errmsg(smpl, errno, NULL, 0, "Failed to evaluate template.");
    smpl->errors = NULL;
    return NULL;
}


void smpl_free_errors(char **errors)
{
    char **e;

    if (errors == NULL)
        return;

    for (e = errors; *e != NULL; e++)
        smpl_free(*e);

    smpl_free(errors);
}


int smpl_print_template(smpl_t *smpl, int fd)
{
    SMPL_UNUSED(smpl);
    SMPL_UNUSED(fd);

    return 0;
}


void smpl_dump_template(smpl_t *smpl, int fd)
{
    smpl_macro_t *m = NULL;
    smpl_list_t  *p, *n;

    smpl_list_foreach(&smpl->macros, p, n) {
        m = smpl_list_entry(p, typeof(*m), hook);
        macro_dump(smpl, fd, m);
    }

    if (m != NULL)
        dprintf(fd, "\n");

    block_dump(smpl, fd, &smpl->body, 0);
}


