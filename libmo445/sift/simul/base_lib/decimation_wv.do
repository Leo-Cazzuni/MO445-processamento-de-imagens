onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /tb_decimation/inst_decimation/CLK_I
add wave -noupdate -radix hexadecimal /tb_decimation/inst_decimation/DATA_A_I
add wave -noupdate /tb_decimation/inst_decimation/GATE_I
add wave -noupdate -divider {New Divider}
add wave -noupdate -radix hexadecimal /tb_decimation/inst_decimation/DATA_A_O
add wave -noupdate /tb_decimation/inst_decimation/DATA_VALID_A_O
add wave -noupdate -radix unsigned /tb_decimation/inst_decimation/DECIMATION_I
add wave -noupdate /tb_decimation/timer_s
add wave -noupdate -radix unsigned /tb_decimation/cnt
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {20636907 ps} 0}
configure wave -namecolwidth 172
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
WaveRestoreZoom {20303157 ps} {20931751 ps}
