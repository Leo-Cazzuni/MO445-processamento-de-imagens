onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /dco_tb/mclk_i
add wave -noupdate -divider <NULL>
add wave -noupdate /dco_tb/resync_s
add wave -noupdate /dco_tb/prescale_s
add wave -noupdate -divider <NULL>
add wave -noupdate -expand /dco_tb/dco_clk_s
add wave -noupdate -expand /dco_tb/ref_clk_s
add wave -noupdate -divider <NULL>
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {299 ps} 0}
configure wave -namecolwidth 108
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
WaveRestoreZoom {0 ps} {1095 ps}
