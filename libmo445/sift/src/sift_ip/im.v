module im(
  clk,
  run,
  neighborhood,
  seed,
  conquest,
  state,
  transmit_data
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
   // control inputs
   //
   input clk;
   input run;
   input neighborhood;
   input seed;
   input conquest;
   input [ 1 : 0 ] state;
   //
   // control outputs
   //
   output transmit_data;
   reg [ 3 : 0 ] transmit_data_rg;

   assign transmit_data = transmit_data_rg == 3'h0 ? 1'b0 : 1'b1;

   // process to change state
   always@( posedge clk ) begin
      if ( run == 1 ) begin
         // activete when is a seed
         if ( ( state == STOP_ST ) && ( ( seed == 1'b1 ) || ( conquest == 1'b1 ) ) ) begin
            if ( neighborhood == 0 ) transmit_data_rg <= 4'h4;
            else transmit_data_rg <= 4'h8;
         end
         // activete when conquested
         //else if ( ( state == STOP_ST ) && ( conquest == 1'b1 ) ) begin
            //if ( neighborhood == 0 ) transmit_data_rg <= 4'h4;
            //else transmit_data_rg <= 4'h8;
         //end
         // reduce conter when not conquested
         else if ( ( state == STOP_ST ) && ( conquest == 1'b0 ) ) begin
            if ( transmit_data_rg != 0 ) transmit_data_rg <= transmit_data_rg - 4'h1;
         end
      end
      // deactivete in the beginning
      else transmit_data_rg <= 0;
   end

endmodule // im
