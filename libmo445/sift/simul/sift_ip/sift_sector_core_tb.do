vlib work
vlib sift_ip

onerror {abort}

vlog +acc "../../src/sift_ip/idp.v" 				-work sift_ip
vlog +acc "../../src/sift_ip/ifp.v" 				-work sift_ip
vlog +acc "../../src/sift_ip/ifp_set.v" 			-work sift_ip
vlog +acc "../../src/sift_ip/im.v" 					-work sift_ip
vlog +acc "../../src/sift_ip/sc.v" 					-work sift_ip
vlog +acc "../../src/sift_ip/sift_sector_core.v" 	-work sift_ip

vlog +acc "sift_sector_core_tb.v"

vsim -t 1ps -lib work sift_sector_core_tb

log -r *

#add wave *
do sift_sector_core_wv.do

view structure
view signals

run 1.1 us
