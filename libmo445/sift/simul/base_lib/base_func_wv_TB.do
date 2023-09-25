onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -divider vec_fit
add wave -noupdate /base_func_tb/vec_value_s
add wave -noupdate /base_func_tb/vec_size_s
add wave -noupdate /base_func_tb/vec_fit_err_s
add wave -noupdate -divider bin2gray
add wave -noupdate /base_func_tb/bin_2bit_s
add wave -noupdate /base_func_tb/gray_2bit_s
add wave -noupdate /base_func_tb/bin2gray_2bit_s
add wave -noupdate /base_func_tb/gray2bin_2bit_s
add wave -noupdate /base_func_tb/bin2gray_2bit_err_s
add wave -noupdate /base_func_tb/gray2bin_2bit_err_s
add wave -noupdate /base_func_tb/bin_3bit_s
add wave -noupdate /base_func_tb/gray_3bit_s
add wave -noupdate /base_func_tb/bin2gray_3bit_s
add wave -noupdate /base_func_tb/gray2bin_3bit_s
add wave -noupdate /base_func_tb/bin2gray_3bit_err_s
add wave -noupdate /base_func_tb/gray2bin_3bit_err_s
add wave -noupdate /base_func_tb/bin_4bit_s
add wave -noupdate /base_func_tb/gray_4bit_s
add wave -noupdate /base_func_tb/bin2gray_4bit_s
add wave -noupdate /base_func_tb/gray2bin_4bit_s
add wave -noupdate /base_func_tb/bin2gray_4bit_err_s
add wave -noupdate /base_func_tb/gray2bin_4bit_err_s
add wave -noupdate -divider onehot
add wave -noupdate /base_func_tb/onehot_s
add wave -noupdate /base_func_tb/onehot_expected_s
add wave -noupdate /base_func_tb/onehot_err_s
add wave -noupdate /base_func_tb/bin_s
add wave -noupdate /base_func_tb/bin_expected_s
add wave -noupdate /base_func_tb/bin_err_s
add wave -noupdate -divider maj_vote
add wave -noupdate /base_func_tb/maj_vote_s
add wave -noupdate /base_func_tb/maj_vote_expected_s
add wave -noupdate /base_func_tb/maj_vote_err_s
add wave -noupdate -divider <NULL>
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {301 ps} 0}
configure wave -namecolwidth 176
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
WaveRestoreZoom {0 ps} {1035 ps}
