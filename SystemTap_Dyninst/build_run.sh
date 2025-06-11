make exercise
if [ $? -eq 0 ]; then
    stap --dyninst -c ./exercise.elf count.stp
    stap -v func_time.stp -d ./exercise.elf 'function("print_iteration")'
fi


make count
#TERMINAL 1:
#    ./$exercise.elf
#TERMINAL 2:
#    ./count.elf `pidof exercise.elf` print_iteration

