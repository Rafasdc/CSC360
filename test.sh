

./assign2 3 t0.txt | grep OFF > myt0.txt
./assign2 7 t1.txt | grep OFF > myt1.txt
./assign2 8 t2.txt | grep OFF > myt2.txt
./assign2 7 t3.txt | grep OFF > myt3.txt
./assign2 12 t4.txt | grep OFF > myt4.txt

diff t0_sol.txt myt0.txt
diff t1_sol.txt myt1.txt
diff t2_sol.txt myt2.txt
diff t3_sol.txt myt3.txt
diff t4_sol.txt myt4.txt
