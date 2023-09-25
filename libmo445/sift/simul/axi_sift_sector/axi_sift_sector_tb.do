vlib work
vlib axi_sift_sector

onerror {abort}

vlog +acc "../../src/axi_sift_sector/idp.v" 				-work axi_sift_sector
vlog +acc "../../src/axi_sift_sector/ifp.v" 				-work axi_sift_sector
vlog +acc "../../src/axi_sift_sector/ifp_set.v" 			-work axi_sift_sector
vlog +acc "../../src/axi_sift_sector/im.v" 					-work axi_sift_sector
vlog +acc "../../src/axi_sift_sector/sc.v" 					-work axi_sift_sector
vlog +acc "../../src/axi_sift_sector/axi_sift_sector.v" 	-work axi_sift_sector

vlog +acc "axi_sift_sector_tb.v"

vsim -t 1ps -lib work axi_sift_sector_tb

log -r *

#add wave *
do axi_sift_sector_wv.do

view structure
view signals

run 1.1 us
