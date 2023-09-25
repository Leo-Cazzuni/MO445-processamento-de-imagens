onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /sync_lib_tb/mclk_i
add wave -noupdate /sync_lib_tb/clk_s
add wave -noupdate -divider syncr
add wave -noupdate /sync_lib_tb/sync_clk_s
add wave -noupdate -divider up
add wave -noupdate /sync_lib_tb/clk_up_s
add wave -noupdate -divider {sync up}
add wave -noupdate /sync_lib_tb/sync_up_s
add wave -noupdate -divider down
add wave -noupdate /sync_lib_tb/clk_dn_s
add wave -noupdate -divider {sync down}
add wave -noupdate /sync_lib_tb/sync_dn_s
add wave -noupdate -divider up/down
add wave -noupdate /sync_lib_tb/clk_ud_s
add wave -noupdate -divider {sync up/down}
add wave -noupdate /sync_lib_tb/sync_ud_s
add wave -noupdate -divider {pulse stretcher}
add wave -noupdate /sync_lib_tb/pulse_i
add wave -noupdate /sync_lib_tb/pulse_o
add wave -noupdate -divider <NULL>
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {74179 ps} 0}
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
WaveRestoreZoom {0 ps} {661532 ps}
