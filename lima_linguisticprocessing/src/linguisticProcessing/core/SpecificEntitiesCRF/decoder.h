/*
 *      Wapiti - A linear-chain CRF tool
 *
 * Copyright (c) 2009-2013  CNRS
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef decoder_h
#define decoder_h


#ifdef    __cplusplus
extern "C" {
#endif/*__cplusplus*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "wapiti.h"
#include "model.h"
#include "sequence.h"

void tag_viterbi(mdl_t *mdl, const seq_t *seq,
                 uint32_t out[], double *sc, double psc[]);
  /*
void tag_nbviterbi(mdl_t *mdl, const seq_t *seq, uint32_t N,
                   uint32_t out[][N], double sc[], double psc[][N]);
  */
void tag_label(mdl_t *mdl, FILE *fin, FILE *fout);
void tag_eval(mdl_t *mdl, double *te, double *se);

typedef struct dataSE_s dataSE_t;
struct dataSE_s {
  uint64_t pos;
  uint64_t lgth;
  char* type;
  char* str;
};

typedef struct listDat_s listDat_t;
struct listDat_s {
  listDat_t *next;
  dataSE_t *data;
};

dataSE_t* createDataSE(uint64_t p, uint64_t l, char* ty, char* s);
  void deleteDataSE(dataSE_t *dat);
  
listDat_t* createList(dataSE_t *newData);
listDat_t* deleteFirst(listDat_t *l);
void deleteList(listDat_t *list);
  
listDat_t* addBack(listDat_t *l, dataSE_t *dat);

uint64_t lengthList(listDat_t *l);

void printDataSE(dataSE_t *d);

void printList(listDat_t *l);

  //void tag_label2(mdl_t *mdl, FILE *fin, FILE *fout, listDat_t **lstD, raw_t *r);
  listDat_t* tag_label2(mdl_t *mdl, FILE *fin, FILE *fout, listDat_t **lstD, raw_t *r);
  

#ifdef    __cplusplus
}
#endif/*__cplusplus*/

#endif

