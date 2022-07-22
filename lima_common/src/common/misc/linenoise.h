// Copyright (c) 2010, Pieter Noordhuis <pcnoordhuis at gmail dot com>
// Copyright (c) 2010, Salvatore Sanfilippo <antirez at gmail dot com>
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef __LINENOISE_H
#define __LINENOISE_H


typedef struct linenoiseCompletions {
  size_t len;
  char **cvec;
} linenoiseCompletions;

typedef void(linenoiseCompletionCallback)(const char *, linenoiseCompletions *);
void linenoiseSetCompletionCallback(linenoiseCompletionCallback *);
void linenoiseAddCompletion(linenoiseCompletions *, const char *);

char *linenoise(const char *prompt);
int linenoiseHistoryAdd(const char *line);
int linenoiseHistorySetMaxLen(int len);
int linenoiseHistorySave(char *filename);
int linenoiseHistoryLoad(char *filename);
void linenoiseClearScreen(void);
void linenoiseSetMultiLine(int ml);

#endif /* __LINENOISE_H */
