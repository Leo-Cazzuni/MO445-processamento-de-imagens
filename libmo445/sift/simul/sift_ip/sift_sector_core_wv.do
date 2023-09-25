onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /sift_sector_core_tb/is_stoping_s
add wave -noupdate /sift_sector_core_tb/is_reading_s
add wave -noupdate /sift_sector_core_tb/i
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/clk_i
add wave -noupdate -radix unsigned /sift_sector_core_tb/cmd_reg_i
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_sc/read_en
add wave -noupdate -radix unsigned /sift_sector_core_tb/sift_sector_core_u/U_sc/state
add wave -noupdate -radix hexadecimal {/sift_sector_core_tb/sift_sector_core_u/ifp_set_data_out_s[11:0]}
add wave -noupdate -radix hexadecimal {/sift_sector_core_tb/readdata_o[11:0]}
add wave -noupdate -radix unsigned /sift_sector_core_tb/sift_sector_core_u/U_sc/SC_base_address
add wave -noupdate -radix unsigned /sift_sector_core_tb/sift_sector_core_u/U_sc/SC_address_o
add wave -noupdate /sift_sector_core_tb/test_if_s
add wave -noupdate -radix hexadecimal -childformat {{{/sift_sector_core_tb/result[0][8]} -radix hexadecimal} {{/sift_sector_core_tb/result[0][7]} -radix hexadecimal} {{/sift_sector_core_tb/result[0][6]} -radix hexadecimal} {{/sift_sector_core_tb/result[0][5]} -radix hexadecimal} {{/sift_sector_core_tb/result[0][4]} -radix hexadecimal} {{/sift_sector_core_tb/result[0][3]} -radix hexadecimal} {{/sift_sector_core_tb/result[0][2]} -radix hexadecimal} {{/sift_sector_core_tb/result[0][1]} -radix hexadecimal} {{/sift_sector_core_tb/result[0][0]} -radix hexadecimal}} -subitemconfig {{/sift_sector_core_tb/result[0][8]} {-height 16 -radix hexadecimal} {/sift_sector_core_tb/result[0][7]} {-height 16 -radix hexadecimal} {/sift_sector_core_tb/result[0][6]} {-height 16 -radix hexadecimal} {/sift_sector_core_tb/result[0][5]} {-height 16 -radix hexadecimal} {/sift_sector_core_tb/result[0][4]} {-height 16 -radix hexadecimal} {/sift_sector_core_tb/result[0][3]} {-height 16 -radix hexadecimal} {/sift_sector_core_tb/result[0][2]} {-height 16 -radix hexadecimal} {/sift_sector_core_tb/result[0][1]} {-height 16 -radix hexadecimal} {/sift_sector_core_tb/result[0][0]} {-height 16 -radix hexadecimal}} {/sift_sector_core_tb/result[0]}
add wave -noupdate /sift_sector_core_tb/chipselect_i
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/address_i
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/AVL_read_memory
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/mem_enable
add wave -noupdate -radix hexadecimal {/sift_sector_core_tb/sift_sector_core_u/U_ifp_set/ri[11:0]}
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/central_mem_out
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/left_mem_out
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/right_mem_out
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/up_mem_out
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/down_mem_out
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/up_left_mem_out
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/up_right_mem_out
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/down_left_mem_out
add wave -noupdate /sift_sector_core_tb/sift_sector_core_u/U_ifp_set/down_right_mem_out
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 2} {968900 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 207
configure wave -valuecolwidth 64
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
configure wave -timelineunits us
update
WaveRestoreZoom {968037 ps} {970293 ps}
