./waf --run "scratch/scen2 --DCR=true --sack=true --cong_var=2 --folder_dest=1"
./waf --run "scratch/scen2 --DCR=false --cong_var=1 --folder_dest=2"
./waf --run "scratch/scen2 --DCR=true --sack=true --cong_var=1 --folder_dest=3"
./waf --run "scratch/intermittent --DCR=false --cong_var=2 --folder_dest=1"
./waf --run "scratch/intermittent --DCR=true --sack=true --cong_var=2 --folder_dest=2"
./waf --run "scratch/intermittent --DCR=false --cong_var=1 --folder_dest=3"
./waf --run "scratch/intermittent --DCR=true --sack=true --cong_var=1 --folder_dest=4"
