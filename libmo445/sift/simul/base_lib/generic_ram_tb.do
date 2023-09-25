
add wave -noupdate -expand -group "TB Top" "/*"
add wave -noupdate -expand -group "Ref FF"     "/ref_ff_u/*"
# add wave -noupdate -expand -group "Ref Dist"   "/ref_distributed_u/*"
# add wave -noupdate -expand -group "Ref Block"  "/ref_block_u/*"

add wave -noupdate -expand -group "SV FF"     "/sv_ff_u/*"
add wave -noupdate -expand -group "SV FF"     "sim:/generic_ram_tb/sv_ff_u/ram_sv"
# add wave -noupdate -expand -group "SV FF"     "sim:/generic_ram_tb/sv_ff_u/ram_lock"
# add wave -noupdate -expand -group "SV Dist"   "/sv_distributed_u/*"
# add wave -noupdate -expand -group "SV Block"  "/sv_block_u/*"

configure wave -namecolwidth 300
configure wave -valuecolwidth 150
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

