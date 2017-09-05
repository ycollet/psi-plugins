#!/bin/bash
#awk '/lv2:index $INC/{sub(/$INC/,++i)}1' super_welle.ttl.in > super_welle.ttl.out
awk '$0 = /(.*)\$INC;/ ? "     "$1 " " ++increment ";" : $0' super_welle.ttl.in > super_welle.ttl
