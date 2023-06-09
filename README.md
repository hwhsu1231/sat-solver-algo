# SAT Solver Algorithm

## Overview

This repository contains the implementation of a SAT Solver, a tool designed to solve the Boolean satisfiability problem (SAT).

## Prerequistes

* OS Platform: `Windows`
* CMake version: `>=3.23`
* C/C++ Compiler: `MSVC 16 (VS 2019)`

## Usage

1.  Configure and build the project:

    ```
    cmake --presest default
    cmake --build --preset default-debug
    ```

2.  Execute the `sat-solver` executable:

    ```
    cd build/default/Debug/bin
    sat-solver
    ```

## Hyperparameters

The SAT Solver in this project is implemented with several techniques and heuristics to improve the efficiency of the solving process:

* **Boolean Constraint Propagation (BCP):** 

  A technique used to simplify the given problem based on the values assigned to certain variables.

* **Two Literal Watching:** 

  An optimization technique used to efficiently perform Boolean Constraint Propagation.

* **Variable State Independent Decaying Sum (VSIDS):** 

  A heuristic for deciding the next variable to branch when there is no forced choice.

* **Maximum Occurrences in clauses of Minimum size (MOM):** 

  A heuristic used to select the variable to be assigned in the DPLL algorithm.

* **Jeroslaw-Wang (JW) Score:** 

  A heuristic that assigns a score to each variable based on its occurrence in the clauses.

## Experiment Results

<details><summary>Click to expand</summary>

```
Class: 2nf
---- [01-2-nf-300-var-1000-cls.cnf] Variables: 300, Clauses: 1000
-------- BCP               : UNSATISFIABLE, Time = 9.6715ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 8.9324ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 9.6672ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 9.8985ms
---- [01-2-nf-300-var-1500-cls.cnf] Variables: 300, Clauses: 1500
-------- BCP               : UNSATISFIABLE, Time = 14.9642ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 16.5631ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 14.4263ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 14.2423ms
---- [01-2-nf-300-var-2000-cls.cnf] Variables: 300, Clauses: 2000
-------- BCP               : UNSATISFIABLE, Time = 18.3247ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 21.9464ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 18.7406ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 19.3098ms
---- [01-2-nf-300-var-2500-cls.cnf] Variables: 300, Clauses: 2500
-------- BCP               : UNSATISFIABLE, Time = 24.1046ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 25.3363ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 24.4893ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 24.0813ms
---- [01-2-nf-300-var-3000-cls.cnf] Variables: 300, Clauses: 3000
-------- BCP               : UNSATISFIABLE, Time = 27.8664ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 27.9102ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 28.4853ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 28.4917ms
---- [01-2-nf-300-var-3500-cls.cnf] Variables: 300, Clauses: 3500
-------- BCP               : UNSATISFIABLE, Time = 33.8419ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 33.0017ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 34.8213ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 32.7254ms
---- [01-2-nf-300-var-4000-cls.cnf] Variables: 300, Clauses: 4000
-------- BCP               : UNSATISFIABLE, Time = 40.4065ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 38.823ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 39.7782ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 40.3771ms
Class: 3nf
---- [01-3-nf-300-var-1000-cls.cnf] Variables: 300, Clauses: 1000
-------- BCP               : SATISFIABLE,   Time = 159.453ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 55.1305ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 13.9643ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 11.6227ms
---- [01-3-nf-300-var-1500-cls.cnf] Variables: 300, Clauses: 1500
-------- BCP               : TIMEOUT
-------- BCP + VSIDS       : TIMEOUT
-------- BCP + VSIDS + MOM : TIMEOUT
-------- BCP + VSIDS + JW  : TIMEOUT
---- [01-3-nf-300-var-2000-cls.cnf] Variables: 300, Clauses: 2000
-------- BCP               : UNSATISFIABLE, Time = 8081.39ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 2371.26ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 1443.82ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 1500.98ms
---- [01-3-nf-300-var-2500-cls.cnf] Variables: 300, Clauses: 2500
-------- BCP               : UNSATISFIABLE, Time = 1280.06ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 306.531ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 255.603ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 221.114ms
---- [01-3-nf-300-var-3000-cls.cnf] Variables: 300, Clauses: 3000
-------- BCP               : UNSATISFIABLE, Time = 481.229ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 155.28ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 149.022ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 144.99ms
---- [01-3-nf-300-var-3500-cls.cnf] Variables: 300, Clauses: 3500
-------- BCP               : UNSATISFIABLE, Time = 383.286ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 136.497ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 113.277ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 106.775ms
---- [01-3-nf-300-var-4000-cls.cnf] Variables: 300, Clauses: 4000
-------- BCP               : UNSATISFIABLE, Time = 196.451ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 85.706ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 112.814ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 80.2597ms
Class: 4nf
---- [01-4-nf-300-var-1000-cls.cnf] Variables: 300, Clauses: 1000
-------- BCP               : SATISFIABLE,   Time = 13.0279ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 12.347ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 12.8507ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 12.6082ms
---- [01-4-nf-300-var-1500-cls.cnf] Variables: 300, Clauses: 1500
-------- BCP               : SATISFIABLE,   Time = 21.5079ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 23.2171ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 21.1757ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 27.1239ms
---- [01-4-nf-300-var-2000-cls.cnf] Variables: 300, Clauses: 2000
-------- BCP               : SATISFIABLE,   Time = 62.4661ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 39.6701ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 110.143ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 96.2669ms
---- [01-4-nf-300-var-2500-cls.cnf] Variables: 300, Clauses: 2500
-------- BCP               : TIMEOUT
-------- BCP + VSIDS       : TIMEOUT
-------- BCP + VSIDS + MOM : TIMEOUT
-------- BCP + VSIDS + JW  : TIMEOUT
---- [01-4-nf-300-var-3000-cls.cnf] Variables: 300, Clauses: 3000
-------- BCP               : TIMEOUT
-------- BCP + VSIDS       : TIMEOUT
-------- BCP + VSIDS + MOM : TIMEOUT
-------- BCP + VSIDS + JW  : TIMEOUT
---- [01-4-nf-300-var-3500-cls.cnf] Variables: 300, Clauses: 3500
-------- BCP               : TIMEOUT
-------- BCP + VSIDS       : TIMEOUT
-------- BCP + VSIDS + MOM : TIMEOUT
-------- BCP + VSIDS + JW  : TIMEOUT
---- [01-4-nf-300-var-4000-cls.cnf] Variables: 300, Clauses: 4000
-------- BCP               : TIMEOUT
-------- BCP + VSIDS       : TIMEOUT
-------- BCP + VSIDS + MOM : TIMEOUT
-------- BCP + VSIDS + JW  : TIMEOUT
Class: m2-hard
---- [par16-1-c.cnf] Variables: 317, Clauses: 1264
-------- BCP               : SATISFIABLE,   Time = 1139.64ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 2391.97ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 796.139ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 503.034ms
---- [par16-1.cnf] Variables: 1015, Clauses: 1844
-------- BCP               : SATISFIABLE,   Time = 1495.61ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 2533.99ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 792.689ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 744.089ms
---- [par32-1-c.cnf] Variables: 1315, Clauses: 5254
-------- BCP               : TIMEOUT
-------- BCP + VSIDS       : TIMEOUT
-------- BCP + VSIDS + MOM : TIMEOUT
-------- BCP + VSIDS + JW  : TIMEOUT
---- [par32-1.cnf] Variables: 3176, Clauses: 7460
-------- BCP               : TIMEOUT
-------- BCP + VSIDS       : TIMEOUT
-------- BCP + VSIDS + MOM : TIMEOUT
-------- BCP + VSIDS + JW  : TIMEOUT
Class: m2-simple
---- [aim-100-1_6-no-1.cnf] Variables: 100, Clauses: 160
-------- BCP               : UNSATISFIABLE, Time = 2.8294ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 3.3626ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 2.7234ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 2.6284ms
---- [aim-100-1_6-yes1-1.cnf] Variables: 100, Clauses: 160
-------- BCP               : SATISFIABLE,   Time = 3.7682ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 5.0827ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 2.6181ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 2.7256ms
---- [aim-200-1_6-no-1.cnf] Variables: 200, Clauses: 320
-------- BCP               : UNSATISFIABLE, Time = 5.6574ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 5.0645ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 6.2692ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 5.7258ms
---- [aim-200-1_6-yes1-1.cnf] Variables: 200, Clauses: 320
-------- BCP               : SATISFIABLE,   Time = 8.0405ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 11.2684ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 5.3933ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 5.5703ms
---- [aim-50-1_6-no-1.cnf] Variables: 50, Clauses: 76
-------- BCP               : UNSATISFIABLE, Time = 1.2683ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 1.2799ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 1.8902ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 1.3013ms
---- [aim-50-1_6-yes1-1.cnf] Variables: 50, Clauses: 80
-------- BCP               : SATISFIABLE,   Time = 1.9515ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 1.4381ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 1.5282ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 1.4669ms
---- [dubois100.cnf] Variables: 300, Clauses: 800
-------- BCP               : TIMEOUT
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 1776.08ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 2013.55ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 1981.85ms
---- [dubois20.cnf] Variables: 60, Clauses: 160
-------- BCP               : UNSATISFIABLE, Time = 2573.17ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 87.145ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 99.7269ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 109.109ms
---- [ii16a1.cnf] Variables: 1650, Clauses: 19368
-------- BCP               : SATISFIABLE,   Time = 626.982ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 411.99ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 349.39ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 297.107ms
---- [ii32a1.cnf] Variables: 459, Clauses: 9212
-------- BCP               : SATISFIABLE,   Time = 307.886ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 258.73ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 446.45ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 383.848ms
---- [ii8a1.cnf] Variables: 66, Clauses: 186
-------- BCP               : SATISFIABLE,   Time = 2.666ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 2.4335ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 2.3131ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 2.8796ms
---- [jnh1.cnf] Variables: 100, Clauses: 850
-------- BCP               : SATISFIABLE,   Time = 23.5246ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 22.9971ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 21.8083ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 27.7874ms
---- [jnh10.cnf] Variables: 100, Clauses: 850
-------- BCP               : UNSATISFIABLE, Time = 31.1064ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 22.078ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 26.7131ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 22.4017ms
---- [jnh11.cnf] Variables: 100, Clauses: 850
-------- BCP               : UNSATISFIABLE, Time = 20.02ms
-------- BCP + VSIDS       : UNSATISFIABLE, Time = 25.4044ms
-------- BCP + VSIDS + MOM : UNSATISFIABLE, Time = 38.2282ms
-------- BCP + VSIDS + JW  : UNSATISFIABLE, Time = 26.6638ms
---- [par8-1-c.cnf] Variables: 64, Clauses: 254
-------- BCP               : SATISFIABLE,   Time = 8.8733ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 5.9862ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 7.4637ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 4.778ms
---- [par8-1.cnf] Variables: 350, Clauses: 412
-------- BCP               : SATISFIABLE,   Time = 20.6476ms
-------- BCP + VSIDS       : SATISFIABLE,   Time = 20.1947ms
-------- BCP + VSIDS + MOM : SATISFIABLE,   Time = 19.8803ms
-------- BCP + VSIDS + JW  : SATISFIABLE,   Time = 19.8507ms
```

</details>