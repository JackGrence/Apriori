# Aprior algorithm implementation in C

It's my data mining home work.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

What things you need to install the software and how to install them


```
Give examples
```

### Installing

#### For Linux:

Just `make` is enough.

```
make
```

If you want debug with gdb, you can:

```
make debug
```

#### For Windows:

I just use `x86_64-w64-mingw32-gcc` to compile windows PE file on my Ubuntu:

```
make windows
```

## Running the tests

```
./apriori test.data 2
```

Output:

```
start initialize c
End read.
Find HASH_FUNC_MOD: 5
Find MAX_LEAF_SIZE: 1
Start generate C2, spent: 0.000164s
C2 num: 10
L1 size: 5
Start count C2, spent: 0.000242s
End read.
Start generate L, spent: 0.000312s
++++++++++++++++++
14
++++++++++++++++++
3 Start apriori generation, spent: 0.000398s
Start count_C, spent: 0.000423s
End read.
End loop cycle, spent: 0.000487s
Start generate L, spent: 0.000504s
++++++++++++++++++
20
++++++++++++++++++
4 Start apriori generation, spent: 0.000566s
Start count_C, spent: 0.000581s
End read.
End loop cycle, spent: 0.000664s
Start generate L, spent: 0.000682s
++++++++++++++++++
21
++++++++++++++++++
5 Start apriori generation, spent: 0.000753s
Start count_C, spent: 0.000767s
End read.
End loop cycle, spent: 0.000823s
Total spent time, spent: 0.000839s
frq: 21
```

## Usage

```
./apriori FILENAME MINSUP
```

## Input file format

| User Id | Transaction length | Item IDs                     |
|---------|--------------------|------------------------------|
| 8 Bytes | 4 Bytes            | Transaction length * 4 Bytes |
