set return_code [catch {
    # Upgrade ip
    upgrade_ip [get_ips *]
    
    # Make wrapper
    make_wrapper -files [get_files samknn.srcs/sources_1/bd/design_1/design_1.bd] -top
    add_files -norecurse samknn.gen/sources_1/bd/design_1/hdl/design_1_wrapper.vhd

} result]

# Exit vivado
exit $return_code
