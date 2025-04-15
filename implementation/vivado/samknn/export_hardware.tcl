set return_code [catch {
    # Upgrade ip
    upgrade_ip [get_ips *]

    # launch synthesis
    reset_run synth_1
    launch_runs synth_1
    wait_on_run synth_1

    # Run implementation and generate bitstream
    reset_run impl_1
    launch_runs impl_1 -to_step write_bitstream
    wait_on_run impl_1

    # Write xsa
    write_hw_platform -fixed -force -include_bit -hw platform.xsa

} result]

# Exit vivado
exit $return_code


