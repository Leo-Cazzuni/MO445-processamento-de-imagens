onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider {CRC GEN}
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_gen_u/reset_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_gen_u/clk_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_gen_u/init_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_gen_u/data_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_gen_u/data_valid_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_gen_u/CRC_o
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_gen_u/CRC_s
add wave -noupdate -divider {CRC CHECK}
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/reset_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/clk_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/init_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/data_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/data_valid_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/data_last_i
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/crc_err_valid_o
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/crc_err_o
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/CRC_s
add wave -noupdate -radix hexadecimal /crc_tb/CRC32_check_u/crc_check_en_s
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {90 ns} 0}
configure wave -namecolwidth 278
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {0 ns} {162 ns}
