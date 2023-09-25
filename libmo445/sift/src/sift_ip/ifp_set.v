/***********************************/
// INCLUDES
/***********************************/
`include "ifp.v"
module ifp_set(
  clock,
  AVL_chipselect,
  AVL_write,
  AVL_address,
  AVL_writedata,
  AVL_readdata,
  SC_read,
  SC_send,
  SC_receive,
  SC_write,
  SC_run,
  SC_address,
  SC_pathfunction,
  SC_neighborhood,
  SC_neighbor_address,
  SC_data_type,
  SC_carry_in,
  SM_state,
  SM_direction,
  IFPS_direction,
  IFPS_finish
);

  // primitive parameters
  parameter IFP_LINE = 3;
  parameter SECTOR_LINE = 3;
  parameter SECTOR_BITS = 2;
  parameter NIL = 1024'hFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF;
  parameter ZERO = 1024'h0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000;
  parameter TRANSMIT_SIZE = 8;
  parameter SAVE_SIZE = 24;
  parameter DATA_SIZE = 38;
  parameter STOP_ST = 2'h0;
  parameter COST_ST = 2'h1;
  parameter ROOT_ST = 2'h2;
  parameter SAVE_ST = 2'h3;

  // devived parameters
  parameter ADDR_SIZE = 2 * ( SECTOR_BITS + 6 );
  parameter MEM_SIZE = DATA_SIZE * SECTOR_LINE * SECTOR_LINE;

  // global signals
  input clock;
  // avalon signals
  input AVL_chipselect;
  input AVL_write;
  input AVL_address;
  input [ 31 : 0 ] AVL_writedata;
  output [ 31 : 0 ] AVL_readdata;
  // SC signals
  input SC_write;
  input SC_read;
  input SC_send;
  input SC_receive;
  input SC_run;
  input [ ADDR_SIZE - 1 : 0 ] SC_address;
  input [ 1 : 0 ] SC_pathfunction;
  input SC_neighborhood;
  input SC_data_type;
  input SC_carry_in;
  input SC_neighbor_address;
   // SM  signals
  input [ 1 : 0 ] SM_state;
  input [ 2 : 0 ] SM_direction;
  // IFP set signals
  input IFPS_direction;
  output IFPS_finish;

  // registers
  reg [ IFP_LINE * IFP_LINE + 23 - 1 : 0 ] ri;
  reg [ IFP_LINE - 3 : 0 ] left_rg_in; // boarder input registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] right_rg_in; // boarder input registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] up_rg_in; // boarder input registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] down_rg_in; // boarder input registers for comunication between sectors
  reg up_left_rg_in; // boarder output registers for comunication between sectors
  reg up_right_rg_in; // boarder output registers for comunication between sectors
  reg down_left_rg_in; // boarder output registers for comunication between sectors
  reg down_right_rg_in; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] left_propagate;
  reg [ IFP_LINE - 3 : 0 ] right_propagate;
  reg [ IFP_LINE - 3 : 0 ] up_propagate;
  reg [ IFP_LINE - 3 : 0 ] down_propagate;
  reg up_left_propagate;
  reg up_right_propagate;
  reg down_left_propagate;
  reg down_right_propagate;
  reg [ 1 : 0 ] SM_state_rg; // to work with IFP's state.
  reg [ 2 : 0 ] SM_direction_rg; // to work with IFP's state.
  reg SC_neighborhood_rg;
  reg SC_read_rg;
  reg SC_write_rg;
  reg SC_receive_rg;
  reg SC_send_rg;
  reg SC_run_rg;
  reg SC_data_type_rg;
  reg AVL_chipselect_rg;
  reg AVL_write_rg;
  reg AVL_address_rg;
  reg SC_neighbor_address_rg;
  reg [ 31 : 0 ] AVL_writedata_rg;

  // linkage signals
  wire AVL_write_memory;
  wire AVL_read_memory;
  reg [ IFP_LINE * IFP_LINE - 1 : 0 ] IFPS_data_in_bl;
  reg [ IFP_LINE * IFP_LINE - 1 : 0 ] IFPS_extern_data_in_bl;
  reg [ IFP_LINE * IFP_LINE - 1 : 0 ] IFPS_intern_data_in_bl;
  wire [ IFP_LINE * IFP_LINE - 1 : 0 ] IFPS_data_out_bl;

  // auxiliar
  integer ifp_l, ifp_c;

  reg [ ( IFP_LINE - 2 ) * ( IFP_LINE - 2 ) - 1 : 0 ] central_rg_out;
  reg [ IFP_LINE - 3 : 0 ] left_rg_out; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] right_rg_out; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] up_rg_out; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] down_rg_out; // boarder output registers for comunication between sectors
  reg up_left_rg_out; // boarder output registers for comunication between sectors
  reg up_right_rg_out; // boarder output registers for comunication between sectors
  reg down_left_rg_out; // boarder output registers for comunication between sectors
  reg down_right_rg_out; // boarder output registers for comunication between sectors

  reg [ ( IFP_LINE - 2 ) * ( IFP_LINE - 2 ) - 1 : 0 ] central_mem_in;
  reg [ IFP_LINE - 3 : 0 ] left_mem_in; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] right_mem_in; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] up_mem_in; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] down_mem_in; // boarder output registers for comunication between sectors
  reg up_left_mem_in; // boarder output registers for comunication between sectors
  reg up_right_mem_in; // boarder output registers for comunication between sectors
  reg down_left_mem_in; // boarder output registers for comunication between sectors
  reg down_right_mem_in; // boarder output registers for comunication between sectors

  reg [ ADDR_SIZE - 1 : 0 ] central_mem_address;
  reg [ ADDR_SIZE - 1 : 0 ] left_mem_address; // boarder output registers for comunication between sectors
  reg [ ADDR_SIZE - 1 : 0 ] right_mem_address; // boarder output registers for comunication between sectors
  reg [ ADDR_SIZE - 1 : 0 ] up_mem_address; // boarder output registers for comunication between sectors
  reg [ ADDR_SIZE - 1 : 0 ] down_mem_address; // boarder output registers for comunication between sectors
  reg [ ADDR_SIZE - 1 : 0 ] up_left_mem_address; // boarder output registers for comunication between sectors
  reg [ ADDR_SIZE - 1 : 0 ] up_right_mem_address; // boarder output registers for comunication between sectors
  reg [ ADDR_SIZE - 1 : 0 ] down_left_mem_address; // boarder output registers for comunication between sectors
  reg [ ADDR_SIZE - 1 : 0 ] down_right_mem_address; // boarder output registers for comunication between sectors

//Controls wires
  wire mem_enable;

  reg [ ( IFP_LINE - 2 ) * ( IFP_LINE - 2 ) - 1 : 0 ] central_mem_out;
  reg [ IFP_LINE - 3 : 0 ] left_mem_out; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] right_mem_out; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] up_mem_out; // boarder output registers for comunication between sectors
  reg [ IFP_LINE - 3 : 0 ] down_mem_out; // boarder output registers for comunication between sectors
  reg up_left_mem_out; // boarder output registers for comunication between sectors
  reg up_right_mem_out; // boarder output registers for comunication between sectors
  reg down_left_mem_out; // boarder output registers for comunication between sectors
  reg down_right_mem_out; // boarder output registers for comunication between sectors

  //Memories**********************
  reg[ ( IFP_LINE - 2 ) * ( IFP_LINE - 2 ) - 1 : 0 ] mem_central[ MEM_SIZE - 1 : 0 ];
  reg[ IFP_LINE - 3 : 0 ] mem_left[ MEM_SIZE - 1 : 0 ];
  reg[ IFP_LINE - 3 : 0 ] mem_right[ MEM_SIZE - 1 : 0 ];
  reg[ IFP_LINE - 3 : 0 ] mem_up[ MEM_SIZE - 1 : 0 ];
  reg[ IFP_LINE - 3 : 0 ] mem_down[ MEM_SIZE - 1 : 0 ];
  reg mem_up_left[ MEM_SIZE - 1 : 0 ];
  reg mem_up_right[ MEM_SIZE - 1 : 0 ];
  reg mem_down_left[ MEM_SIZE - 1 : 0 ];
  reg mem_down_right[ MEM_SIZE - 1 : 0 ];

    ifp U_ifp_0_0 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( IFPS_data_out_bl[ 0 * IFP_LINE + 1 ] ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 0 * IFP_LINE + 0 ] ),
      .data_out( IFPS_data_out_bl[ 0 * IFP_LINE + 0 ] )
    );

    ifp U_ifp_0_1 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( IFPS_data_out_bl[ 0 * IFP_LINE + 2 ] ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 0 * IFP_LINE + 1 ] ),
      .data_out( IFPS_data_out_bl[ 0 * IFP_LINE + 1 ] )
    );

    ifp U_ifp_0_2 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( 1'b0 ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 0 * IFP_LINE + 2 ] ),
      .data_out( IFPS_data_out_bl[ 0 * IFP_LINE + 2 ] )
    );

    ifp U_ifp_1_0 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( IFPS_data_out_bl[ 1 * IFP_LINE + 1 ] ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 1 * IFP_LINE + 0 ] ),
      .data_out( IFPS_data_out_bl[ 1 * IFP_LINE + 0 ] )
    );

    ifp U_ifp_1_1 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( IFPS_data_out_bl[ 1 * IFP_LINE + 2 ] ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 1 * IFP_LINE + 1 ] ),
      .data_out( IFPS_data_out_bl[ 1 * IFP_LINE + 1 ] )
    );

    ifp U_ifp_1_2 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( 1'b0 ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 1 * IFP_LINE + 2 ] ),
      .data_out( IFPS_data_out_bl[ 1 * IFP_LINE + 2 ] )
    );

    ifp U_ifp_2_0 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( IFPS_data_out_bl[ 2 * IFP_LINE + 1 ] ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 2 * IFP_LINE + 0 ] ),
      .data_out( IFPS_data_out_bl[ 2 * IFP_LINE + 0 ] )
    );

    ifp U_ifp_2_1 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( IFPS_data_out_bl[ 2 * IFP_LINE + 2 ] ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 2 * IFP_LINE + 1 ] ),
      .data_out( IFPS_data_out_bl[ 2 * IFP_LINE + 1 ] )
    );

    ifp U_ifp_2_2 (
      // global signals
      .clock( clock ),
      // SC signals
      .run( SC_run ),
      .mem_send( SC_send ),
      .mem_receive( SC_receive ),
      .pathfunction( SC_pathfunction ),
      .neighborhood( SC_neighborhood ),
      .carry_in( SC_carry_in ),
      .data_type( SC_data_type ),
      // SM signals
      .state( SM_state ),
      .direction( IFPS_direction ),
      // counter signals
      // neighbors signals
      .transmit_data_in( 1'b0 ),
      // data_signals
      .data_in( IFPS_data_in_bl[ 2 * IFP_LINE + 2 ] ),
      .data_out( IFPS_data_out_bl[ 2 * IFP_LINE + 2 ] )
    );

  // assign auxiliar signals from AVL to IFPs
  assign AVL_write_memory = ( ( AVL_write_rg == 1'b1 ) && ( AVL_writedata_rg[ 1 : 0 ] == 2'b01 ) && ( AVL_address_rg == 1'b0 ) ) ? 1'b1 : 1'b0;
  assign AVL_read_memory = ( ( AVL_write_rg == 1'b1 ) && ( AVL_writedata_rg[ 1 : 0 ] == 2'b00 ) && ( AVL_address_rg == 1'b0 ) ) ? 1'b1 : 1'b0;

  // assign of ri
  always@( posedge clock ) begin
    // IFTs read command
    if ( AVL_read_memory == 1 ) begin
      for ( ifp_l = 0; ifp_l < IFP_LINE - 2; ifp_l = ifp_l + 1 ) begin
        for ( ifp_c = 0; ifp_c < IFP_LINE - 2; ifp_c = ifp_c + 1 ) begin
          ri[ ( ifp_l + 1 ) * 3 + 1 + ifp_c ] <= central_mem_out[ ifp_c + ifp_l * 1 ];
        end
        ri[ ( ifp_l + 1 ) * IFP_LINE ] <= left_mem_out[ ifp_l ];
        ri[ ( ifp_l + 2 ) * IFP_LINE - 1 ] <= right_mem_out[ ifp_l ];
        ri[ ifp_l + 1 ] <= up_mem_out[ ifp_l ];
        ri[ ifp_l + IFP_LINE * ( IFP_LINE - 1 ) + 1 ] <= down_mem_out[ ifp_l ];
      end
      ri[ 0 ] <= up_left_mem_out;
      ri[ IFP_LINE - 1 ] <= up_right_mem_out;
      ri[ IFP_LINE * ( IFP_LINE - 1 ) ] <= down_left_mem_out;
      ri[ IFP_LINE * IFP_LINE - 1 ] <= down_right_mem_out;
    end 
    // DATA read/write command
    else if ( ( AVL_chipselect_rg == 1 ) && ( AVL_address_rg == 1 ) ) begin
      ri[ 31 : 0 ] <= AVL_writedata[ 31 : 0 ];
    end 
  end

  // memories address assignment
  always@( posedge clock ) begin
    central_mem_address <= SC_address;
    // iqual to central_mem_address when AVL write and before receive internal_data 
    if ( SC_neighbor_address_rg == 1'b0 ) begin
      left_mem_address <= SC_address;
      right_mem_address <= SC_address;
      up_mem_address <= SC_address;
      down_mem_address <= SC_address;
      up_left_mem_address <= SC_address;
      up_right_mem_address <= SC_address;
      down_left_mem_address <= SC_address;
      down_right_mem_address <= SC_address;
    end
    // get neighbor sectors to propagate 
    else begin
      left_mem_address <= SC_address + DATA_SIZE;
      right_mem_address <= SC_address - DATA_SIZE;
      up_mem_address <= SC_address + DATA_SIZE * SECTOR_LINE;
      down_mem_address <= SC_address - DATA_SIZE * SECTOR_LINE;
      if ( SM_direction_rg[ 2 : 0 ] == 3'h0 ) begin
        up_left_mem_address <= SC_address + DATA_SIZE * SECTOR_LINE;
        up_right_mem_address <= SC_address + DATA_SIZE * SECTOR_LINE - DATA_SIZE;
        down_right_mem_address <= SC_address - DATA_SIZE;
      end
      if ( SM_direction_rg[ 2 : 0 ] == 3'h1 ) begin
        up_right_mem_address <= SC_address - DATA_SIZE;
        down_right_mem_address <= SC_address - DATA_SIZE;
      end
      if ( SM_direction_rg[ 2 : 0 ] == 3'h2 ) begin
        up_right_mem_address <= SC_address - DATA_SIZE;
        down_left_mem_address <= SC_address - DATA_SIZE * SECTOR_LINE;
        down_right_mem_address <= SC_address - DATA_SIZE * SECTOR_LINE - DATA_SIZE;
      end
      if ( SM_direction_rg[ 2 : 0 ] == 3'h3 ) begin
        down_left_mem_address <= SC_address - DATA_SIZE * SECTOR_LINE;
        down_right_mem_address <= SC_address - DATA_SIZE * SECTOR_LINE;
      end
      if ( SM_direction_rg[ 2 : 0 ] == 3'h4 ) begin
        up_left_mem_address <= SC_address + DATA_SIZE;
        down_left_mem_address <= SC_address - DATA_SIZE * SECTOR_LINE + DATA_SIZE;
        down_right_mem_address <= SC_address - DATA_SIZE * SECTOR_LINE;
      end
      if ( SM_direction_rg[ 2 : 0 ] == 3'h5 ) begin
        up_left_mem_address <= SC_address + DATA_SIZE;
        down_left_mem_address <= SC_address + DATA_SIZE;
      end
      if ( SM_direction_rg[ 2 : 0 ] == 3'h6 ) begin
        up_left_mem_address <= SC_address + DATA_SIZE * SECTOR_LINE + DATA_SIZE;
        up_right_mem_address <= SC_address + DATA_SIZE * SECTOR_LINE;
        down_left_mem_address <= SC_address + DATA_SIZE;
      end
      if ( SM_direction_rg[ 2 : 0 ] == 3'h7 ) begin
        up_left_mem_address <= SC_address + DATA_SIZE * SECTOR_LINE;
        up_right_mem_address <= SC_address + DATA_SIZE * SECTOR_LINE;
      end
    end
  end

  assign mem_enable = ( SC_write_rg | AVL_write_memory == 1 ) ? 1'b1 : 1'b0;

  // memories input regiesters assignment
  always@* begin
    if ( ( AVL_write_rg == 1'b1 ) && ( AVL_writedata_rg[ 1 ] == 1'b0 ) ) begin // reading from AVL
      for ( ifp_l = 0; ifp_l < IFP_LINE - 2; ifp_l = ifp_l + 1 ) begin
        for ( ifp_c = 0; ifp_c < IFP_LINE - 2; ifp_c = ifp_c + 1 ) begin
          central_mem_in[ ifp_c + ifp_l * 1 ] <= ri[ ( ifp_l + 1 ) * 3 + 1 + ifp_c ];
        end
        left_mem_in[ ifp_l ] <= ri[ ( ifp_l + 1 ) * IFP_LINE ];
        right_mem_in[ ifp_l ] <= ri[ ( ifp_l + 2 ) * IFP_LINE - 1 ];
        up_mem_in[ ifp_l ] <= ri[ ifp_l + 1 ];
        down_mem_in[ ifp_l ] <= ri[ ifp_l + IFP_LINE * ( IFP_LINE - 1 ) + 1 ];
      end
      up_left_mem_in <= ri[ 0 ];
      up_right_mem_in <= ri[ IFP_LINE - 1 ];
      down_left_mem_in <= ri[ IFP_LINE * ( IFP_LINE - 1 ) ];
      down_right_mem_in <= ri[ IFP_LINE * IFP_LINE - 1 ];
    end
    else begin // reading from IFP'S
      central_mem_in <= central_rg_out;
      left_mem_in <= left_rg_out;
      right_mem_in <= right_rg_out;
      up_mem_in <= up_rg_out;
      down_mem_in <= down_rg_out;
      up_left_mem_in <= up_left_rg_out;
      up_right_mem_in <= up_right_rg_out;
      down_left_mem_in <= down_left_rg_out;
      down_right_mem_in <= down_right_rg_out;
    end
  end

  always@( posedge clock ) begin
    if ( mem_enable == 1 ) begin
      mem_central[ central_mem_address ] <= central_mem_in;
      mem_left[ left_mem_address ] <= left_mem_in;
      mem_right[ right_mem_address ] <= right_mem_in;
      mem_up[ up_mem_address ] <= up_mem_in;
      mem_down[ down_mem_address ] <= down_mem_in;
      mem_up_left[ up_left_mem_address ] <= up_left_mem_in;
      mem_up_right[ up_right_mem_address ] <= up_right_mem_in;
      mem_down_left[ down_left_mem_address ] <= down_left_mem_in;
      mem_down_right[ down_right_mem_address ] <= down_right_mem_in;
    end
    central_mem_out <= mem_central[ central_mem_address ];
    left_mem_out <= mem_left[ left_mem_address ];
    right_mem_out <= mem_right[ right_mem_address ];
    up_mem_out <= mem_up[ up_mem_address ];
    down_mem_out <= mem_down[ down_mem_address ];
    up_left_mem_out <= mem_up_left[ up_left_mem_address ];
    up_right_mem_out <= mem_up_right[ up_right_mem_address ];
    down_left_mem_out <= mem_down_left[ down_left_mem_address ];
    down_right_mem_out <= mem_down_right[ down_right_mem_address ];
  end

  //assign propagation registers from borders
  always@( posedge clock ) begin
    if ( ( SC_run_rg == 1'b1 ) && ( SM_state_rg == STOP_ST ) ) begin // write to IFPs
      for ( ifp_l = 0; ifp_l < IFP_LINE - 2; ifp_l = ifp_l + 1 ) begin
        if ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) < DATA_SIZE ) left_propagate[ ifp_l ] <= 1'b0;
        else left_propagate[ ifp_l ] <= right_mem_out[ ifp_l ];
        if ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) >= ( SECTOR_LINE - 1 ) * DATA_SIZE ) right_propagate[ ifp_l ] <= 1'b0;
        else right_propagate[ ifp_l ] <= left_mem_out[ ifp_l ];
        if ( central_mem_address < SECTOR_LINE * DATA_SIZE ) up_propagate[ ifp_l ] <= 1'b0;
        else up_propagate[ ifp_l ] <= down_mem_out[ ifp_l ];
        if ( central_mem_address >= ( SECTOR_LINE - 1 ) * SECTOR_LINE * DATA_SIZE ) down_propagate[ ifp_l ] <= 1'b0;
        else down_propagate[ ifp_l ] <= up_mem_out[ ifp_l ];
      end
      if ( ( ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) >= DATA_SIZE ) && ( central_mem_address >= SECTOR_LINE * DATA_SIZE ) && ( SM_direction_rg[ 2 : 0 ] == 3'h2 ) ) || ( ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) >= DATA_SIZE ) && ( SM_direction_rg[ 2 : 1 ] == 2'h0 ) ) || ( ( central_mem_address >= SECTOR_LINE * DATA_SIZE ) && ( ( SM_direction_rg[ 2 : 0 ] == 3'h3 ) || ( SM_direction_rg[ 2 : 0 ] == 3'h4 ) ) ) ) up_left_propagate <= down_right_mem_out;
      else up_left_propagate <= 1'b0;
      if ( ( ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) < ( SECTOR_LINE - 1 ) * DATA_SIZE ) && ( central_mem_address >= SECTOR_LINE * DATA_SIZE ) && ( SM_direction_rg[ 2 : 0 ] == 3'h4 ) ) || ( ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) < ( SECTOR_LINE - 1 ) * DATA_SIZE ) &&  ( ( SM_direction_rg[ 2 : 0 ] == 3'h5 ) || ( SM_direction_rg[ 2 : 0 ] == 3'h6 ) ) ) || ( ( central_mem_address >= SECTOR_LINE * DATA_SIZE ) && ( ( SM_direction_rg[ 2 : 0 ] == 3'h2 ) || ( SM_direction_rg[ 2 : 0 ] == 3'h3 ) ) ) ) up_right_propagate <= down_left_mem_out;
      else up_right_propagate <= 1'b0;
      if ( ( ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) >= DATA_SIZE ) && ( central_mem_address < ( SECTOR_LINE - 1 ) * SECTOR_LINE * DATA_SIZE ) && ( SM_direction_rg[ 2 : 0 ] == 3'h0 ) ) || ( ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) >= DATA_SIZE ) && ( ( SM_direction_rg[ 2 : 0 ] == 3'h1 ) || ( SM_direction_rg[ 2 : 0 ] == 3'h2 ) ) ) || ( ( central_mem_address < ( SECTOR_LINE - 1 ) * SECTOR_LINE * DATA_SIZE ) && ( SM_direction_rg[ 2 : 1 ] == 2'h3 ) ) )down_left_propagate <= up_right_mem_out;
      else down_left_propagate <= 1'b0;
      if ( ( ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) < ( SECTOR_LINE - 1 ) * DATA_SIZE ) && ( central_mem_address < ( SECTOR_LINE - 1 ) * SECTOR_LINE * DATA_SIZE ) && ( SM_direction_rg[ 2 : 0 ] == 3'h6 ) ) || ( ( central_mem_address % ( SECTOR_LINE * DATA_SIZE ) < ( SECTOR_LINE - 1 ) * DATA_SIZE ) &&  ( SM_direction_rg[ 2 : 1 ] == 2'h2 ) ) || ( ( central_mem_address < ( SECTOR_LINE - 1 ) * SECTOR_LINE * DATA_SIZE ) && ( ( SM_direction_rg[ 2 : 0 ] == 3'h7 ) || ( SM_direction_rg[ 2 : 0 ] == 3'h8 ) ) ) ) down_right_propagate <= up_left_mem_out;
      else down_right_propagate <= 1'b0;
    end
    else if ( SC_run_rg == 1'b0 ) begin 
      left_propagate <= 0;
      right_propagate <= 0;
      up_propagate <= 0;
      down_propagate <= 0;
      up_left_propagate <= 1'b0;
      up_right_propagate <= 1'b0;
      down_left_propagate <= 1'b0;
      down_right_propagate <= 1'b0;
    end
  end

  //assign correct data for propagation
  always@* begin
    left_rg_in <= ( ~left_propagate ) | right_mem_out;
    right_rg_in <= ( ~right_propagate ) | left_mem_out;
    up_rg_in <= ( ~up_propagate ) | down_mem_out;
    down_rg_in <= ( ~down_propagate ) | up_mem_out;
    up_left_rg_in <= ( ~up_left_propagate ) | down_right_mem_out;
    up_right_rg_in <= ( ~up_right_propagate ) | down_left_mem_out;
    down_left_rg_in <= ( ~down_left_propagate ) | up_right_mem_out;
    down_right_rg_in <= ( ~down_right_propagate ) | up_left_mem_out;
  end

   // assign IFPS_external_total_data_in
  always@* begin
    for ( ifp_l = 0; ifp_l < IFP_LINE ; ifp_l = ifp_l + 1 ) begin
      for ( ifp_c = 0; ifp_c < IFP_LINE ; ifp_c = ifp_c + 1 ) begin
        if ( SM_direction_rg[ 2 : 0 ] == 3'h1 ) begin // left data in right propagation
          if ( ( ifp_c == 0 ) && ( ifp_l == 0 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_left_rg_in;
          else if ( ( ifp_c == 0 ) && ( ifp_l == IFP_LINE - 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_left_rg_in;
          else if ( ifp_c == 0 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= left_rg_in[ ifp_l - 1 ];
          else IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= IFPS_data_out_bl[ ifp_l * IFP_LINE + ifp_c - 1 ];
        end
        else if ( SM_direction_rg[ 2 : 0 ] == 3'h5 ) begin // right data in left propagation
          if ( ( ifp_c == IFP_LINE - 1 ) && ( ifp_l == 0 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_right_rg_in;
          else if ( ( ifp_c == IFP_LINE - 1 ) && ( ifp_l == IFP_LINE - 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_right_rg_in;
          else if ( ifp_c == IFP_LINE - 1 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= right_rg_in[ ifp_l - 1 ];
          else IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= IFPS_data_out_bl[ ifp_l * IFP_LINE + ifp_c + 1 ];
        end
        else if ( SM_direction_rg[ 2 : 0 ] == 3'h3 ) begin // up data in down propagation
          if ( ( ifp_l == 0 ) && ( ifp_c == 0 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_right_rg_in;
          else if ( ( ifp_l == 0 ) && ( ifp_c == IFP_LINE - 1 ) )  IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_left_rg_in;
          else if ( ifp_l == 0 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_rg_in[ ifp_c - 1 ];
          else IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= IFPS_data_out_bl[ ( ifp_l - 1 ) * IFP_LINE + ifp_c ];
        end
        else if ( SM_direction_rg[ 2 : 0 ] == 3'h7 ) begin // down data in up propagation
          if ( ( ifp_l == IFP_LINE - 1 ) && ( ifp_c == 0 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_right_rg_in;
          else if ( ( ifp_l == IFP_LINE - 1 ) && ( ifp_c == IFP_LINE - 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_left_rg_in;
          else if ( ifp_l == IFP_LINE - 1 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_rg_in[ ifp_c - 1 ];
          else IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= IFPS_data_out_bl[ ( ifp_l + 1 ) * IFP_LINE + ifp_c ];
        end
        else if ( SM_direction_rg[ 2 : 0 ] == 3'h0 ) begin // down left data in up right propagation
          if ( ( ifp_c == 0 ) && ( ifp_l == IFP_LINE - 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_left_rg_in;
          else if ( ( ifp_c == 0 ) && ( ifp_l == IFP_LINE - 2 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_left_rg_in;
          else if ( ( ifp_c == 1 ) && ( ifp_l == IFP_LINE - 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_right_rg_in;
          else if ( ifp_c == 0 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= left_rg_in[ ifp_l ];
          else if ( ifp_l == IFP_LINE - 1 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_rg_in[ ifp_c - 2 ];
          else IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= IFPS_data_out_bl[ ( ifp_l + 1 ) * IFP_LINE + ifp_c - 1 ];
        end
        else if ( SM_direction_rg[ 2 : 0 ] == 3'h4 ) begin // up right data in down left propagation
          if ( ( ifp_c == IFP_LINE - 1 ) && ( ifp_l == 0 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_right_rg_in;
          else if ( ( ifp_c == IFP_LINE - 2 ) && ( ifp_l == 0 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_left_rg_in;
          else if ( ( ifp_c == IFP_LINE - 1 ) && ( ifp_l == 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_right_rg_in;
          else if ( ifp_c == IFP_LINE - 1 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= right_rg_in[ ifp_l - 2 ];
          else if ( ifp_l == 0 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_rg_in[ ifp_c ];
          else IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= IFPS_data_out_bl[ ( ifp_l - 1 ) * IFP_LINE + ifp_c + 1 ];
        end
        else if ( SM_direction_rg[ 2 : 0 ] == 3'h2 ) begin // up left data in down right propagation
          if ( ( ifp_l == 0 ) && ( ifp_c == 0 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_left_rg_in;
          else if ( ( ifp_l == 1 ) && ( ifp_c == 0 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_left_rg_in;
          else if ( ( ifp_l == 0 ) && ( ifp_c == 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_right_rg_in;
          else if ( ifp_l == 0 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_rg_in[ ifp_c - 2 ];
          else if ( ifp_c == 0 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= left_rg_in[ ifp_l - 2 ];
          else IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= IFPS_data_out_bl[ ( ifp_l - 1 ) * IFP_LINE + ifp_c - 1 ];
        end
        else begin // down right data in up left propagation
          if ( ( ifp_l == IFP_LINE - 1 ) && ( ifp_c == IFP_LINE - 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_right_rg_in;
          else if ( ( ifp_l == IFP_LINE - 2 ) && ( ifp_c == IFP_LINE - 1 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= up_right_rg_in;
          else if ( ( ifp_l == IFP_LINE - 1 ) && ( ifp_c == IFP_LINE - 2 ) ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_left_rg_in;
          else if ( ifp_l == IFP_LINE - 1 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= down_rg_in[ ifp_c ];
          else if ( ifp_c == IFP_LINE - 1 ) IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= right_rg_in[ ifp_l ];
          else IFPS_extern_data_in_bl[ ifp_l * IFP_LINE + ifp_c ] <= IFPS_data_out_bl[ ( ifp_l + 1 ) * IFP_LINE + ifp_c + 1 ];
        end
      end
    end
  end

  // assign of IFPS_intern_data_in_bl
  always@( posedge clock ) begin
    // IFTs read command
    if ( SC_read_rg == 1'b1 ) begin
      for ( ifp_l = 0; ifp_l < IFP_LINE ; ifp_l = ifp_l + 1 ) begin
        for ( ifp_c = 0; ifp_c < IFP_LINE ; ifp_c = ifp_c + 1 ) begin
          if ( ( ifp_c == IFP_LINE - 1 ) && ( ifp_l == IFP_LINE - 1 ) ) IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= down_right_mem_out;
          else if ( ( ifp_c == 0 ) && ( ifp_l == 0 ) ) IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= up_left_mem_out;
          else if ( ( ifp_c == 0 ) && ( ifp_l == IFP_LINE - 1 ) ) IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= down_left_mem_out;
          else if ( ( ifp_c == IFP_LINE - 1 ) && ( ifp_l == 0 ) ) IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= up_right_mem_out;
          else if ( ifp_l == IFP_LINE - 1 ) IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= down_mem_out[ ifp_c - 1 ];
          else if ( ifp_c == IFP_LINE - 1 ) IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= right_mem_out[ ifp_l - 1 ];
          else if ( ifp_l == 0 ) IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= up_mem_out[ ifp_c - 1 ];
          else if ( ifp_c == 0 ) IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= left_mem_out[ ifp_l - 1 ];
          else IFPS_intern_data_in_bl[ ifp_l * 3 + ifp_c ] <= central_mem_out[ ( ifp_l - 1 ) * 1 + ifp_c - 1 ];
        end 
      end 
    end 
  end 

  // assign of IFPS_data_in_bl
  always@* begin
    if ( SC_run_rg == 1'b1 ) begin
      IFPS_data_in_bl <= IFPS_extern_data_in_bl;
    end 
    else begin
      IFPS_data_in_bl <= IFPS_intern_data_in_bl;
    end 
  end 

  // Assign of register out from left, right up and down.
  always@( posedge clock ) begin
    if ( SC_receive_rg == 1'b1 ) begin
      for ( ifp_l = 0; ifp_l < IFP_LINE - 2; ifp_l = ifp_l + 1 ) begin
        for ( ifp_c = 0; ifp_c < IFP_LINE - 2; ifp_c = ifp_c + 1 ) begin
          central_rg_out[ ifp_c + ifp_l * 1 ] <= IFPS_data_out_bl[ ( ifp_l + 1 ) * 3 + ifp_c + 1 ];
        end
        left_rg_out[ ifp_l ] <= IFPS_data_out_bl[ ( ifp_l + 1 ) * IFP_LINE ];
        right_rg_out[ ifp_l ] <= IFPS_data_out_bl[ ( ifp_l + 2 ) * IFP_LINE - 1 ];
        up_rg_out[ ifp_l ] <= IFPS_data_out_bl[ ifp_l + 1 ];
        down_rg_out[ ifp_l ] <= IFPS_data_out_bl[ IFP_LINE * ( IFP_LINE - 1 ) + ifp_l + 1 ];
      end
      up_left_rg_out <= IFPS_data_out_bl[ 0 ];
      up_right_rg_out <= IFPS_data_out_bl[ IFP_LINE - 1 ];
      down_left_rg_out <= IFPS_data_out_bl[ IFP_LINE * ( IFP_LINE - 1 ) ];
      down_right_rg_out <= IFPS_data_out_bl[ IFP_LINE * IFP_LINE - 1 ];
    end
  end

  // assign output of inc_counter for SC
  assign IFPS_finish = ( ( IFPS_data_out_bl[ IFP_LINE * IFP_LINE - 1 : 0 ] == ZERO[ IFP_LINE * IFP_LINE - 1 : 0 ] ) && ( SC_run_rg == 1'b1 ) ) ? 1'b1 : 1'b0;

  // assign of data out
  assign AVL_readdata = ri[ 31 : 0 ];

   // register basic signals from SM and SC
    always@( posedge clock ) begin
       SC_data_type_rg <= SC_data_type;
       SC_neighborhood_rg <= SC_neighborhood;
       SC_read_rg <= SC_read;
       SC_write_rg <= SC_write;
       SC_receive_rg <= SC_receive;
       SC_send_rg <= SC_send;
       SC_run_rg <= SC_run;
       SC_neighbor_address_rg <= SC_neighbor_address;
       SM_state_rg <= SM_state;
       SM_direction_rg <= SM_direction;
       AVL_chipselect_rg <= AVL_chipselect;
       AVL_write_rg <= AVL_write;
       AVL_address_rg <= AVL_address;
       AVL_writedata_rg <= AVL_writedata;
    end

endmodule // ifp_set
