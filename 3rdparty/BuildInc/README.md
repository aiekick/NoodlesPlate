## BuildInc

cet utilitaire sert a incrementer des variables dans un fichier include de Build

## Syntaxe

la syntaxe est : 

```
BuildInc rule include_file
```

## Rule

la syntaxe de la rule est : max_build_number:max_minor_number

par exemple une rule de 1000:10 va faire :

```
    if (BuildNumber > 1000) 
    	MinorNumber++;
    if (MinorNumber > 10) 
    	MajorNumber++;
```
   
## Include File

par ex un fichier Build.h
qui contiendra :

```
#pragma once

#define BuildNumber 119
#define MinorNumber 5
#define MajorNumber 0
#define BuildId "0.5.119"
```




