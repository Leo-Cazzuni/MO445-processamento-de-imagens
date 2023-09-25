onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider interface
add wave -noupdate /axi_sift_sector_tb/is_stoping_s
add wave -noupdate /axi_sift_sector_tb/is_reading_s
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_sc/read_en
add wave -noupdate -radix hexadecimal {/axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/m_axis_tdata_o[11:0]}
add wave -noupdate -radix unsigned /axi_sift_sector_tb/axi_sift_sector_u/U_sc/SC_base_address
add wave -noupdate -radix unsigned /axi_sift_sector_tb/axi_sift_sector_u/U_sc/SC_address_o
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/axis_clk_i
add wave -noupdate -radix unsigned /axi_sift_sector_tb/axi_sift_sector_u/U_sc/cmd_reg_s
add wave -noupdate -radix unsigned /axi_sift_sector_tb/axi_sift_sector_u/U_sc/state
add wave -noupdate /axi_sift_sector_tb/test_if_s
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/read_en
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/mem_en
add wave -noupdate -radix hexadecimal {/axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/ri[11:0]}
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/central_mem_out
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/left_mem_out
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/right_mem_out
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/up_mem_out
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/down_mem_out
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/up_left_mem_out
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/up_right_mem_out
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/down_left_mem_out
add wave -noupdate /axi_sift_sector_tb/axi_sift_sector_u/U_ifp_set/down_right_mem_out
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 2} {128 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 193
configure wave -valuecolwidth 41
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
WaveRestoreZoom {0 ps} {1128 ps}
