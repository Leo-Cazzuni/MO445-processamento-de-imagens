/**********************************************************************
 * Project: Silicon Image-Forest Transform - Modelo Setorial
 * Autor: Fabio Cappabianco
 * Date: 2016
 * 
 * wsantos@20180201 - adaptação para interface axi stream.
**********************************************************************/


/***********************************
 * INCLUDES
************************************/

/***********************************
 * MODULE SIFT CONTROLLER
************************************/
module sc(
   axis_clk_i,
   axis_rst_i,
   //
   s_axis_tvalid_i,
   s_axis_tlast_i,
   m_axis_tready_i,
   //   
   IFPS_finish_i,
   IFPS_direction_o,
   SC_read_o,
   SC_send_o,
   SC_receive_o,
   SC_write_o,
   SC_run_o,
   SC_address_o,
   SC_pathfunction_o,
   SC_data_type_o,
   SC_carry_in_o,
   SC_neighborhood_o,
   SC_neighbor_address_o,
   SM_state_o,
   SM_direction_o,
   //
   type_reg_i,
   run_end_o
);
   parameter STOP_ST = 2'b00;
   parameter COST_ST = 2'b01;
   parameter ROOT_ST = 2'b10;
   parameter SAVE_ST = 2'b11;
   parameter C8L16 = 1'b0;
   parameter C16L8 = 1'b1;
   parameter SECTOR_LINE = 4;
   parameter SECTOR_BITS = 2;
   parameter START_ST = 3'h0;
   parameter READ_ST = 3'h1;
   parameter SEND_ST = 3'h2;
   parameter PREPARE_ST = 3'h3;
   parameter RUN_ST = 3'h4;
   parameter RECEIVE_ST = 3'h5;
   parameter WRITE_ST = 3'h6;
   parameter WAIT_ST = 3'h7;
   parameter LEFT_DIR = 1'b0;
   parameter RIGHT_DIR = 1'b1;
   parameter DATA_SIZE = 38;
   parameter SAVE_SIZE = 24;
   parameter MAX_BASE_ADDRESS = (SECTOR_LINE*SECTOR_LINE - 1)*DATA_SIZE;

   input axis_clk_i;
   input axis_rst_i;
   input s_axis_tvalid_i;
   input s_axis_tlast_i;
   input m_axis_tready_i;
   // IFP set signals
   input IFPS_finish_i;
   output IFPS_direction_o;
   // SC and State Machine signals
   output SC_read_o;
   output SC_send_o;
   output SC_receive_o;
   output SC_write_o;
   output SC_run_o;
   output [2*(SECTOR_BITS+6)-1:0] SC_address_o;
   output [1:0] SC_pathfunction_o;
   output SC_data_type_o;
   output SC_neighborhood_o;
   output SC_neighbor_address_o;
   output SC_carry_in_o;
   output [1:0] SM_state_o;
   output [2:0] SM_direction_o;
   // register and interrupt interface
   input type_reg_i;
   output run_end_o;

   // wires
   wire write_en;
   wire read_en;

   // registers
   reg [31:0] m_axis_tdata_s;
   reg [2*(SECTOR_BITS+6)-1:0] SC_base_address;
   reg [2*(SECTOR_BITS+6)-1:0] SC_base_address_tst;
   reg [5:0] SC_offset_address;
   reg [SECTOR_BITS*SECTOR_BITS+1:0] status_count;
   reg [3:0] state;
   reg raster_direction; // direction of raster: left_up or right_down
   reg [3:0] IFPS_direction_rg;
   reg IFPS_finish_rg;
   reg IFPS_changed_rg;
   reg SC_carry_in_o;
   reg ran; // signal to axis_rst_i address after running algorithm
   reg [1:0] wait_propagation; // reg to wait border propagation
   reg SC_neighbor_address_o;
   reg [1:0] SM_state_o; // The register can contain 4 states	
   reg [2:0] SM_direction_o; // The register can contain up to 8 directions

   reg s_axis_tvalid_s;
   reg s_axis_tvalid_sp_s;
   reg s_axis_tlast_s;
   reg s_axis_tlast_sp_s;

   reg m_axis_tvalid_s;

   reg [3:0] cmd_reg_s = 4'b0000;
   wire run_end_s;
   reg run_end_sp_s;
   reg run_end_up_s;


/*****************************************************************
   Asynchronous logic
******************************************************************/

   // outputs for IFP's commands
   assign SC_read_o = (state == READ_ST) ? 1'b1:1'b0;
   assign SC_send_o = (state == SEND_ST) ? 1'b1:1'b0;
   assign SC_receive_o = (state == RECEIVE_ST) ? 1'b1:1'b0;
   assign SC_write_o = (state == WRITE_ST) ? 1'b1:1'b0;
   assign SC_run_o = (state == RUN_ST) ? 1'b1:1'b0;
   assign SC_neighborhood_o = cmd_reg_s[1];
   assign SC_pathfunction_o = cmd_reg_s[3:2];
   assign SC_data_type_o = type_reg_i;
   assign SC_address_o = SC_base_address + SC_offset_address;

   // assign auxiliar signals from AVL to IFPs
   assign write_en = s_axis_tvalid_i;
   assign read_en  = m_axis_tready_i;// && m_axis_tvalid_s;
   assign run_end_o = run_end_sp_s;
   

/*****************************************************************
   Synchronous logic
******************************************************************/

   // Input sample (sanity...)
   always@(posedge axis_clk_i) begin
      s_axis_tvalid_s      <= s_axis_tvalid_i;
      s_axis_tvalid_sp_s   <= s_axis_tvalid_s;
      s_axis_tlast_s       <= s_axis_tlast_i;
      s_axis_tlast_sp_s    <= s_axis_tlast_s;
      run_end_sp_s         <= run_end_s;
   end

   // Imitating command signal previously comming from testbench controls.
   always@(posedge axis_clk_i) begin
      if (axis_rst_i)
         cmd_reg_s <= 4'b0000;
      else
         if (s_axis_tlast_sp_s == 1)
            cmd_reg_s <= 4'b0011;
         else if (run_end_sp_s == 1'b1)
            cmd_reg_s <= 4'b0010;
   end   

   // carry in attribution to set to 1 when root computation starts
   always@(posedge axis_clk_i) begin
      if (axis_rst_i == 1'b1) begin
         SC_carry_in_o <= 1'b0;
      end
      else begin
         if (state == RUN_ST && SM_state_o == COST_ST && SC_offset_address == {1'b0, type_reg_i, ~type_reg_i, 3'h7})
            SC_carry_in_o <= 1'b1;
         else
            SC_carry_in_o <= 1'b0;
      end
   end

   // block describes state transitions
   always@ (posedge axis_clk_i) begin
      if (axis_rst_i == 1'b1) begin
         SM_state_o <= STOP_ST;
      end
      else begin
         if (state == RUN_ST) begin
            if (SM_state_o == STOP_ST && (IFPS_finish_i == 1'b0 || wait_propagation != 2'h0))
               SM_state_o <= COST_ST;
            else if (SM_state_o == COST_ST && SC_offset_address == {2'h1, type_reg_i, 3'h0})
               SM_state_o <= ROOT_ST;
            else if (SM_state_o == ROOT_ST && SC_offset_address == 6'h08)
               SM_state_o <= SAVE_ST;
            else if (SM_state_o == SAVE_ST && SC_offset_address == 6'h25)
               SM_state_o <= STOP_ST;
         end
         else
            SM_state_o <= STOP_ST;
      end
   end

   // assign of direction_rg
   always@ (posedge axis_clk_i) begin
      if (state != RUN_ST) begin
         if (raster_direction == LEFT_DIR)
            SM_direction_o <= 3'h5;
         else
            SM_direction_o <= 3'h1;
      end
      else if (SC_offset_address == 6'h22) begin
         if (cmd_reg_s[1] == 1'b0)
            SM_direction_o[2:0] <= SM_direction_o[2:0] + 3'h2;
         else
            SM_direction_o[2:0] <= SM_direction_o[2:0] + 3'h1;
      end
   end

   // ifps' direction register
   always@ (posedge axis_clk_i) begin
      if (state != RUN_ST) begin
         if (raster_direction == LEFT_DIR)
            IFPS_direction_rg <= 4'h1;
         else
            IFPS_direction_rg <= 4'h5;
      end
      else begin
         if (SC_offset_address == 6'h22) begin
            if (cmd_reg_s[1] == 1'b0)
               IFPS_direction_rg[2:0] <= IFPS_direction_rg[2:0] + 3'h2;
            else
               IFPS_direction_rg[2:0] <= IFPS_direction_rg[2:0] + 3'h1;
         end
         else if (SM_state_o == ROOT_ST)
            IFPS_direction_rg <= {IFPS_direction_rg[0], IFPS_direction_rg[3:1]};
      end
   end
   assign IFPS_direction_o = IFPS_direction_rg[0];

   // organize state on microprogram
   always@ (posedge axis_clk_i) begin
      if (cmd_reg_s[0] == 1'b0) begin
         state                   <= START_ST;
         SC_neighbor_address_o   <= 1'b0;
         ran                     <= 1'b0;
      end
      else begin
         case (state)
            START_ST: begin
               state <= READ_ST;
            end
            READ_ST: begin
               state <= SEND_ST;
               if (SC_offset_address == DATA_SIZE-1)
                  SC_neighbor_address_o <= 1'b1;
            end
            SEND_ST: begin
               if (SC_neighbor_address_o == 1'b0)
                  state <= READ_ST;
               else
                  state <= PREPARE_ST;
            end
            PREPARE_ST: begin
               state <= RUN_ST;
            end
            RUN_ST: begin
               ran <= 1'b1;
               if (SM_state_o == STOP_ST && IFPS_finish_rg == 1'b1 && wait_propagation == 2'h0) begin
                  state <= RECEIVE_ST;
                  SC_neighbor_address_o <= 1'b0;
               end
            end
            RECEIVE_ST: begin
               ran <= 1'b0;
               state <= WRITE_ST;
            end
            WRITE_ST: begin
               if (SC_offset_address != DATA_SIZE - 1)
                  state <= RECEIVE_ST;
               else
                  state <= WAIT_ST;
            end
            default: begin
               if (status_count == 0)
                  state <= WAIT_ST;
               else
                  state <= READ_ST;
            end
         endcase
      end
   end

   // adjust the m_axis_tdata_s for internal use and....
   always@ (posedge axis_clk_i) begin
      m_axis_tdata_s <= {status_count, state};
   end
   assign run_end_s = 1'b1 ? m_axis_tdata_s == 7 : 1'b0;


   // adjust the SC_base_address
   always@ (posedge axis_clk_i) begin
      if (axis_rst_i == 1'b1 ) begin
         SC_base_address <= 0;
      end
      else if (m_axis_tdata_s == 7) begin
         SC_base_address <= 0;
      end
      // increment from avalon write/read to/from ifps
      else if ((write_en == 1'b1 && SC_offset_address == DATA_SIZE-1) ||
               (read_en  == 1'b1 && SC_offset_address == 6'h23)) begin
         if (SC_base_address == MAX_BASE_ADDRESS) begin
            SC_base_address <= 0;
         end
         else begin
            SC_base_address <= SC_base_address + DATA_SIZE;
         end
      end
      // increment from IFP_SET write in microprogram
      else if (state == WRITE_ST && SC_offset_address == 6'h25) begin
         if (raster_direction == LEFT_DIR) begin
            SC_base_address <= SC_base_address - DATA_SIZE;
         end
         else begin
            SC_base_address <= SC_base_address + DATA_SIZE;
         end
      end
   end

   // assign of SC_offset_address
   always@ (posedge axis_clk_i) begin
      if (axis_rst_i == 1'b1)
         SC_offset_address <= 6'h00;
      else begin
         if (state == START_ST) begin // changes in avl write
            if (write_en == 1'b1) begin
               if (SC_offset_address != DATA_SIZE-1)
                  SC_offset_address <= SC_offset_address + 6'h01;
               else
                  SC_offset_address <= 6'h00;
            end
            if (read_en == 1'b1) begin
               if (SC_offset_address != 6'h23)
                  SC_offset_address <= SC_offset_address + 6'h01;
               else
                  SC_offset_address <= 6'h08;
            end
         end
         if (state == READ_ST) begin // changes in mem read
            if (SC_offset_address != DATA_SIZE-1)
               SC_offset_address <= SC_offset_address + 6'h01;
         end
         if (state == PREPARE_ST) begin // changes in prepare
            SC_offset_address <= 6'h08;
         end
         else if (state == RUN_ST) begin
            if (SM_state_o == STOP_ST) begin // changes living stop state to mem receive or cost
               SC_offset_address <= SC_offset_address + 6'h01;
            end
            else if (SM_state_o == COST_ST) begin // changes in cost state
               SC_offset_address <= SC_offset_address + 6'h01;
            end
            else if (SM_state_o == ROOT_ST) begin // changes in root state
               if (SC_offset_address == 6'h1F)
                  SC_offset_address <= 6'h08;
               else
                  SC_offset_address <= SC_offset_address + 6'h01;
            end
            else begin // changes in save state
               if (SC_offset_address == 6'h23)
                  SC_offset_address <= 6'h25;
               else if (SC_offset_address == 6'h25)
                  SC_offset_address <= 6'h08;
               else
                  SC_offset_address <= SC_offset_address + 6'h01;
            end
         end
         else if (state == RECEIVE_ST) begin
            if (ran == 1'b1)
               SC_offset_address <= 6'h00;
         end
         else if (state == WRITE_ST) begin
            if (SC_offset_address != 6'h25)
               SC_offset_address <= SC_offset_address + 6'h01;
            else
               SC_offset_address <= 6'h00;
         end
         else if (state == WAIT_ST) begin
            if (status_count == 0 )
               SC_offset_address <= 6'h08;
         end
      end
   end
   
   // select raster_direction of propagation
   always@ (posedge axis_clk_i) begin
      // restart by axis_rst_i from avalon
      if (axis_rst_i == 1'b1) begin
         raster_direction <= RIGHT_DIR;
      end
      else if (state == WRITE_ST) begin
         if (raster_direction == RIGHT_DIR && SC_base_address == MAX_BASE_ADDRESS)
            raster_direction <= LEFT_DIR;
         if (raster_direction == LEFT_DIR && SC_base_address == 0)
            raster_direction <= RIGHT_DIR;
      end
   end   

   // adjust the status_count
   always@ (posedge axis_clk_i) begin
      if (axis_rst_i == 1'b1) begin
         status_count <= 2*SECTOR_LINE*SECTOR_LINE - 1;
      end
      else if (SM_state_o == STOP_ST && IFPS_changed_rg == 1'b1 && state == RUN_ST) begin
         status_count <= 2*SECTOR_LINE*SECTOR_LINE - 1;
      end
      else if (state == WAIT_ST && cmd_reg_s[0] == 1'b1) begin
         if (status_count != 0)
            status_count <= status_count - 1;
      end
   end

   // wait propagation
   always@ (posedge axis_clk_i) begin
      if (ran == 1'b0) begin
         wait_propagation <= {cmd_reg_s[1], 1'b1};
      end
      else if (SM_state_o == STOP_ST && state == RUN_ST) begin
         if (wait_propagation != 2'h0)
            wait_propagation <= wait_propagation - 2'h1;
      end
   end

   // adjust the IFPS_finish_rg and IFPS_changed_rg
   always@ (posedge axis_clk_i) begin
      if (state != RUN_ST) begin
         IFPS_changed_rg <= 1'b0;
      end
      else begin
         IFPS_changed_rg <= ~IFPS_finish_i;
      end
   end

   always@ (posedge axis_clk_i) begin
      if (state != RUN_ST) begin
         IFPS_finish_rg <= 1'b0;
      end
      else begin
         if (wait_propagation == 2'h0)
            IFPS_finish_rg <= IFPS_finish_i;
         else
            IFPS_finish_rg <= 1'b0;
      end
   end

endmodule
