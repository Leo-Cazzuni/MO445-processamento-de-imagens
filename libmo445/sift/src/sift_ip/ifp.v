/***********************************/
// INCLUDES
/***********************************/
`include "idp.v"
`include "im.v"

module ifp(
	clock,
	run,
	pathfunction,
	neighborhood,
	data_type,
	carry_in,
	state,
	direction,
	mem_send,
   mem_receive,
	transmit_data_in,
	data_in,
	data_out
);

   //
   // parameters
   //
   parameter STOP_ST = 2'b00;
   parameter COST_ST = 2'b01;
   parameter ROOT_ST = 2'b10;
   parameter SAVE_ST = 2'b11;
   parameter C8L16 = 1'b0;
   parameter C16L8 = 1'b1;
   //
   // inputs
   //
   input clock;
   input run;
   input [ 1 : 0 ] pathfunction;
   input mem_send;
   input mem_receive;
   input transmit_data_in;
   input carry_in;
   input neighborhood;
   input data_type;
   input [ 1 : 0 ] state;
   input direction;
   input data_in;
   //
   // outputs
   //
   output data_out;
   reg data_out;
   //
   // signals
   //
   wire result_data;
   wire transmit_data;
   reg [ 1 : 0 ] intern_data;
   //
   // registers
   //
   reg [ 7 : 0 ] zero;
   reg [ 7 : 0 ] bright;
   reg [ 7 : 0 ] cost;
   reg [ 7 : 0 ] complement;
   reg [ 7 : 0 ] label;
   reg [ 3 : 0 ] predecessor;
   reg seed;
   reg changed;
   //
   // passage registers
   //
   reg [ 1 : 0 ] state_rg;
   reg [ 1 : 0 ] pathfunction_rg;
   reg neighborhood_rg;
   reg run_rg;
   reg carry_in_rg;
   reg direction_rg;
   reg data_type_rg;
   reg mem_send_rg;
   reg mem_receive_rg;
   //reg data_in_rg;
   reg transmit_data_out;
   
   idp U_idp( // instance of dp component
      .clock( clock ),
      .pathfunction( pathfunction_rg[ 1 ] ),
      .state( state_rg ),
      .direction( direction_rg ),
      .root_carry_in( carry_in_rg ),
      .extern_data( data_in ),
      .intern_data( intern_data ),
      .result_data( result_data ),
      .conquest( conquest )
   );

   im U_im( // instance of im component
      .clk( clock ),
      .run( run_rg ),
      .neighborhood( neighborhood_rg ),
      .seed( seed ),
      .conquest( conquest ),
      .state( state_rg ),
      .transmit_data( transmit_data )
   );

   // assign SC and SM controls output
   always@( posedge clock ) begin
      state_rg <= state;
      pathfunction_rg <= pathfunction;
      neighborhood_rg <= neighborhood;
      run_rg <= run;
      data_type_rg <= data_type;
      mem_send_rg <= mem_send;
      mem_receive_rg <= mem_receive;
      direction_rg <= direction;
      carry_in_rg <= carry_in;
   end

   // assign intern_data to idp
   always@* begin
      if ( state_rg == COST_ST ) begin
         if ( pathfunction_rg == 2'h2 ) intern_data[ 0 ] <= zero[ 0 ];
         else intern_data[ 0 ] <= bright[ 0 ];
         intern_data[ 1 ] <= cost[ 0 ];
      end
      else if ( state_rg == ROOT_ST ) begin
         if ( data_type_rg == C8L16 ) intern_data[ 0 ] <= complement[ 0 ];
         else intern_data[ 0 ] <= label[ 0 ];
         intern_data[ 1 ] <= predecessor[ 0 ];
      end
      else if ( state_rg == STOP_ST ) begin
         intern_data[ 0 ] <= ~pathfunction_rg[ 1 ];
         intern_data[ 1 ] <= 1'b1;
      end
      else begin // state SAVE_ST
         intern_data[ 0 ] <= cost[ 0 ];
         intern_data[ 1 ] <= 1'b1;
      end
   end

   // rotate registers depending on status
   always@( posedge clock ) begin
      if ( ( mem_send_rg == 1'b1 ) || ( mem_receive_rg == 1'b1 ) ) begin
         changed <= data_in;
         seed <= changed;
         predecessor[ 3 : 0 ] <= { seed, predecessor[ 3 : 1 ] };
         label[ 7 : 0 ] <= { predecessor[ 0 ], label[ 7 : 1 ] };
         complement[ 7 : 0 ] <= { label[ 0 ], complement[ 7 : 1 ] };
         cost[ 7 : 0 ] <= { complement[ 0 ], cost[ 7 : 1 ] };
         bright[ 7 : 0 ] <= { cost[ 0 ], bright[ 7 : 1 ] };
         zero[ 7 : 0 ] <= { 1'b0,  zero[ 7 : 1 ] };
      end
      else if ( run_rg == 1 ) begin
         if ( transmit_data == 1'b1 ) changed <= 1'b1;
         if ( state_rg == COST_ST ) begin
            if ( data_type_rg == C8L16 ) begin
               bright[ 7 : 0 ] <= { bright[ 0 ], bright[ 7 : 1 ] };
               cost[ 7 : 0 ] <= { cost[ 0 ], cost[ 7 : 1 ] };
            end
            else begin
               bright[ 7 : 0 ] <= { zero[ 0 ], bright[ 7 : 1 ] };
               zero[ 7 : 0 ] <= { bright[ 0 ], zero[ 7 : 1 ] };
               cost[ 7 : 0 ] <= { complement[ 0 ], cost[ 7 : 1 ] };
               complement[ 7 : 0 ] <= { cost[ 0 ], complement[ 7 : 1 ] };
            end
         end
         else if ( state_rg == ROOT_ST ) begin
            predecessor[ 3 : 0 ] <= { predecessor[ 0 ], predecessor[ 3 : 1 ] };
            if ( data_type_rg == C16L8 ) label[ 7 : 0 ] <= { label[ 0 ], label[ 7 : 1 ] };
            else begin
               label[ 7 : 0 ] <= { complement[ 0 ], label[ 7 : 1 ] };
               complement[ 7 : 0 ] <= { label[ 0 ], complement[ 7 : 1 ] };
            end
         end
         else if ( state_rg == SAVE_ST ) begin
            seed <= 1'b0;
            predecessor[ 3 : 0 ] <= { result_data, predecessor[ 3 : 1 ] };
            label[ 7 : 0 ] <= { predecessor[ 0 ], label[ 7 : 1 ] };
            complement[ 7 : 0 ] <= { label[ 0 ], complement[ 7 : 1 ] };
            cost[ 7 : 0 ] <= { complement[ 0 ], cost[ 7 : 1 ] };
         end
         else begin // STOP_ST
            if ( pathfunction_rg == 2'h2 ) zero[ 2 : 0 ] <= { 1'b1, ~direction, 1'b1 };
            else zero[ 2 : 0 ] <= 3'h0;
         end
      end
   end

   // assign data_out
   always@* begin
      if ( run_rg == 0 ) data_out <= bright[ 0 ];
      else begin
         if ( state_rg == COST_ST ) begin
            if ( transmit_data == 1'b1 ) data_out <= cost[ 0 ];
            else data_out <= 1'b1;
         end
         else if ( state_rg == ROOT_ST ) begin
            if ( data_type_rg == C8L16 ) data_out <= complement[ 0 ];
            else data_out <= label[ 0 ];
         end
         else data_out <= transmit_data_out;
      end
   end

   // assign transmit_data_out
   always@( posedge clock ) begin
       transmit_data_out <= transmit_data | transmit_data_in;
   end

	
endmodule // ifp
