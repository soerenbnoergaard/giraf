cat << EOF > tmp.csv
0, 0.1
1, 0.2
2, -0.2
3, -0.3
4, 0.0
5, 0.1
6, 0.7
7, -1.0
EOF

cat tmp.csv | ../giraf
