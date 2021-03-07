export ram_str=$(export v=$(export myvar=`free | head -n 2 | tail -n 1 | sed 's/Mem://g'` | echo $myvar | sed 's/ /\n/g' | head -n 2 | tail -n 1) \
    && python -c "print(str($v // 1024) + 'MiB')")

export gpu_temp=$(export gpu_temp_c=$(export x=`nvidia-smi -q | grep Temp | grep Current | head -n 1` && echo $x | sed 's/GPU Current Temp : //g'| sed 's/ C//g') \
        && python -c "print(($gpu_temp_c * (9/5))+32)")


echo $ram_str
echo $gpu_temp
