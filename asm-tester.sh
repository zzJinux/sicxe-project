for entry in test_asms/*.asm
do
  printf "test on $entry:\n"
  printf "assemble $entry\nquit" | ./20131604.out 
  printf "\n"
done