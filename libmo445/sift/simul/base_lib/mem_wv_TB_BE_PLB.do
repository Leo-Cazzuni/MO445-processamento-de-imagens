onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider {PORT A}
add wave -noupdate /mem_tb_be_plb/dualport_be_ram_u/rsta_i
add wave -noupdate /mem_tb_be_plb/dualport_be_ram_u/clka_i
add wave -noupdate /mem_tb_be_plb/dualport_be_ram_u/wrena_i
add wave -noupdate /mem_tb_be_plb/dualport_be_ram_u/rdena_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/dualport_be_ram_u/dataa_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/dualport_be_ram_u/dataa_o
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/dualport_be_ram_u/addra_i
add wave -noupdate -divider {PORT B}
add wave -noupdate /mem_tb_be_plb/dualport_be_ram_u/rstb_i
add wave -noupdate /mem_tb_be_plb/dualport_be_ram_u/clkb_i
add wave -noupdate /mem_tb_be_plb/dualport_be_ram_u/wrenb_i
add wave -noupdate /mem_tb_be_plb/dualport_be_ram_u/rdenb_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/dualport_be_ram_u/datab_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/dualport_be_ram_u/datab_o
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/dualport_be_ram_u/addrb_i
add wave -noupdate -divider PLB
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/plb_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/plb_o
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/rst_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/clk_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/wren_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/rden_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/data_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/data_o
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/addr_i
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/sl_addrack_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/sl_wrdack_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/sl_rddack_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/int_wrdbus_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/int_rddbus_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/reg_rddbus_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/int_abus_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/int_rnw_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/int_be_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/rdcomp_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/tdp_b_ack_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/tdp_b_be_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/tdp_b_rddata_s
add wave -noupdate -radix hexadecimal /mem_tb_be_plb/test_plb_u/tdp_b_wraddr_s
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {229362 ps} 0} {{Cursor 2} {105815 ps} 0} {{Cursor 3} {135847 ps} 0}
configure wave -namecolwidth 150
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 1
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {75243 ps} {266048 ps}
