%%
[ \t]+                  /* ignore whitespace */;
[a-zA-Z0-9\,';:]+       { return 1; }
\.                      { return 1; }
\n                      { yylineno++; }
.                       ; /* Throw everything else away */
