// PLL.h
// Runs on LM4F120/TM4C123
// A software function to change the bus frequency using the PLL.
// Daniel Valvano
// September 10, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Program 2.10, Figure 2.37

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// The #define statement SYSDIV2 initializes
// the PLL to the desired frequency.
#define SYSDIV2 7
// bus frequency is 400MHz/(SYSDIV2+1) = 400MHz/(7+1) = 50 MHz

// configure the system to get its clock from the PLL
void PLL_Init(uint32_t freq);
#define Bus80MHz     4
#define Bus80_000MHz 4
#define Bus66_667MHz 5
#define Bus50_000MHz 7
#define Bus50MHz     7
#define Bus44_444MHz 8
#define Bus40_000MHz 9
#define Bus40MHz     9
#define Bus36_364MHz 10
#define Bus33_333MHz 11
#define Bus30_769MHz 12
#define Bus28_571MHz 13
#define Bus26_667MHz 14
#define Bus25_000MHz 15
#define Bus25MHz     15
#define Bus23_529MHz 16
#define Bus22_222MHz 17
#define Bus21_053MHz 18
#define Bus20_000MHz 19
#define Bus20MHz     19
#define Bus19_048MHz 20
#define Bus18_182MHz 21
#define Bus17_391MHz 22
#define Bus16_667MHz 23
#define Bus16_000MHz 24
#define Bus16MHz     24
#define Bus15_385MHz 25
#define Bus14_815MHz 26
#define Bus14_286MHz 27
#define Bus13_793MHz 28
#define Bus13_333MHz 29
#define Bus12_903MHz 30
#define Bus12_500MHz 31
#define Bus12_121MHz 32
#define Bus11_765MHz 33
#define Bus11_429MHz 34
#define Bus11_111MHz 35
#define Bus10_811MHz 36
#define Bus10_526MHz 37
#define Bus10_256MHz 38
#define Bus10_000MHz 39
#define Bus10MHz     39
#define Bus9_756MHz 40
#define Bus9_524MHz 41
#define Bus9_302MHz 42
#define Bus9_091MHz 43
#define Bus8_889MHz 44
#define Bus8_696MHz 45
#define Bus8_511MHz 46
#define Bus8_333MHz 47
#define Bus8_163MHz 48
#define Bus8_000MHz 49
#define Bus8MHz     49
#define Bus7_843MHz 50
#define Bus7_692MHz 51
#define Bus7_547MHz 52
#define Bus7_407MHz 53
#define Bus7_273MHz 54
#define Bus7_143MHz 55
#define Bus7_018MHz 56
#define Bus6_897MHz 57
#define Bus6_780MHz 58
#define Bus6_667MHz 59
#define Bus6_557MHz 60
#define Bus6_452MHz 61
#define Bus6_349MHz 62
#define Bus6_250MHz 63
#define Bus6_154MHz 64
#define Bus6_061MHz 65
#define Bus5_970MHz 66
#define Bus5_882MHz 67
#define Bus5_797MHz 68
#define Bus5_714MHz 69
#define Bus5_634MHz 70
#define Bus5_556MHz 71
#define Bus5_479MHz 72
#define Bus5_405MHz 73
#define Bus5_333MHz 74
#define Bus5_263MHz 75
#define Bus5_195MHz 76
#define Bus5_128MHz 77
#define Bus5_063MHz 78
#define Bus5_000MHz 79
#define Bus4_938MHz 80
#define Bus4_878MHz 81
#define Bus4_819MHz 82
#define Bus4_762MHz 83
#define Bus4_706MHz 84
#define Bus4_651MHz 85
#define Bus4_598MHz 86
#define Bus4_545MHz 87
#define Bus4_494MHz 88
#define Bus4_444MHz 89
#define Bus4_396MHz 90
#define Bus4_348MHz 91
#define Bus4_301MHz 92
#define Bus4_255MHz 93
#define Bus4_211MHz 94
#define Bus4_167MHz 95
#define Bus4_124MHz 96
#define Bus4_082MHz 97
#define Bus4_040MHz 98
#define Bus4_000MHz 99
#define Bus4MHz     99
#define Bus3_960MHz 100
#define Bus3_922MHz 101
#define Bus3_883MHz 102
#define Bus3_846MHz 103
#define Bus3_810MHz 104
#define Bus3_774MHz 105
#define Bus3_738MHz 106
#define Bus3_704MHz 107
#define Bus3_670MHz 108
#define Bus3_636MHz 109
#define Bus3_604MHz 110
#define Bus3_571MHz 111
#define Bus3_540MHz 112
#define Bus3_509MHz 113
#define Bus3_478MHz 114
#define Bus3_448MHz 115
#define Bus3_419MHz 116
#define Bus3_390MHz 117
#define Bus3_361MHz 118
#define Bus3_333MHz 119
#define Bus3_306MHz 120
#define Bus3_279MHz 121
#define Bus3_252MHz 122
#define Bus3_226MHz 123
#define Bus3_200MHz 124
#define Bus3_175MHz 125
#define Bus3_150MHz 126
#define Bus3_125MHz 127

/*
SYSDIV2  Divisor  Clock (MHz)
 0        1       reserved
 1        2       reserved
 2        3       reserved
 3        4       reserved
 4        5       80.000
 5        6       66.667
 6        7       reserved
 7        8       50.000
 8        9       44.444
 9        10      40.000
 10       11      36.364
 11       12      33.333
 12       13      30.769
 13       14      28.571
 14       15      26.667
 15       16      25.000
 16       17      23.529
 17       18      22.222
 18       19      21.053
 19       20      20.000
 20       21      19.048
 21       22      18.182
 22       23      17.391
 23       24      16.667
 24       25      16.000
 25       26      15.385
 26       27      14.815
 27       28      14.286
 28       29      13.793
 29       30      13.333
 30       31      12.903
 31       32      12.500
 32       33      12.121
 33       34      11.765
 34       35      11.429
 35       36      11.111
 36       37      10.811
 37       38      10.526
 38       39      10.256
 39       40      10.000
 40       41      9.756
 41       42      9.524
 42       43      9.302
 43       44      9.091
 44       45      8.889
 45       46      8.696
 46       47      8.511
 47       48      8.333
 48       49      8.163
 49       50      8.000
 50       51      7.843
 51       52      7.692
 52       53      7.547
 53       54      7.407
 54       55      7.273
 55       56      7.143
 56       57      7.018
 57       58      6.897
 58       59      6.780
 59       60      6.667
 60       61      6.557
 61       62      6.452
 62       63      6.349
 63       64      6.250
 64       65      6.154
 65       66      6.061
 66       67      5.970
 67       68      5.882
 68       69      5.797
 69       70      5.714
 70       71      5.634
 71       72      5.556
 72       73      5.479
 73       74      5.405
 74       75      5.333
 75       76      5.263
 76       77      5.195
 77       78      5.128
 78       79      5.063
 79       80      5.000
 80       81      4.938
 81       82      4.878
 82       83      4.819
 83       84      4.762
 84       85      4.706
 85       86      4.651
 86       87      4.598
 87       88      4.545
 88       89      4.494
 89       90      4.444
 90       91      4.396
 91       92      4.348
 92       93      4.301
 93       94      4.255
 94       95      4.211
 95       96      4.167
 96       97      4.124
 97       98      4.082
 98       99      4.040
 99       100     4.000
 100      101     3.960
 101      102     3.922
 102      103     3.883
 103      104     3.846
 104      105     3.810
 105      106     3.774
 106      107     3.738
 107      108     3.704
 108      109     3.670
 109      110     3.636
 110      111     3.604
 111      112     3.571
 112      113     3.540
 113      114     3.509
 114      115     3.478
 115      116     3.448
 116      117     3.419
 117      118     3.390
 118      119     3.361
 119      120     3.333
 120      121     3.306
 121      122     3.279
 122      123     3.252
 123      124     3.226
 124      125     3.200
 125      126     3.175
 126      127     3.150
 127      128     3.125
*/
