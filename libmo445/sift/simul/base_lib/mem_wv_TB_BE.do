onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider RAMB36
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/RAMB36_inst/CLKA
add wave -noupdate /mem_tb_be/dualport_be_ram_u/RAMB36_inst/WEA
add wave -noupdate /mem_tb_be/dualport_be_ram_u/RAMB36_inst/ENA
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/RAMB36_inst/DIA
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/RAMB36_inst/DOA
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/RAMB36_inst/ADDRA
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/RAMB36_inst/CLKB
add wave -noupdate /mem_tb_be/dualport_be_ram_u/RAMB36_inst/WEB
add wave -noupdate /mem_tb_be/dualport_be_ram_u/RAMB36_inst/ENB
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/RAMB36_inst/DIB
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/RAMB36_inst/DOB
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/RAMB36_inst/ADDRB
add wave -noupdate -divider {New Divider}
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/rsta_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/clka_i
add wave -noupdate -radix binary /mem_tb_be/dualport_be_ram_u/wrena_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/rdena_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/dataa_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/dataa_o
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/addra_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/rstb_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/clkb_i
add wave -noupdate -radix binary /mem_tb_be/dualport_be_ram_u/wrenb_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/rdenb_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/datab_i
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/datab_o
add wave -noupdate -radix hexadecimal /mem_tb_be/dualport_be_ram_u/addrb_i
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {348263 ps} 0}
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
WaveRestoreZoom {413640 ps} {589676 ps}
