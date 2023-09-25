module idp(
	clk_i,
	pathfunction,
	state,
	direction,
	root_carry_in,
	extern_data,
	intern_data,
	result_data,
	conquest
);

	parameter STOP_ST = 2'b00;
	parameter COST_ST = 2'b01;
	parameter ROOT_ST = 2'b10;
	parameter SAVE_ST = 2'b11;
	parameter C8L16 = 1'b0;
	parameter C16L8 = 1'b1;
	// control inputs
	input clk_i;
	input pathfunction;
	input [1 : 0] state;
	input direction;
	// data inputs
	input root_carry_in;
	input extern_data;
	input [1 : 0] intern_data;
	// data outputs
	output result_data;
	reg result_data;
	output conquest;
	reg conquest;

	// registers
	
	reg [27 : 0] result;
	reg [7 : 0] bright;
	wire a1;
	wire b1;
	wire pre_a2;
	wire a2;
	wire b2;
	reg c1;
	reg c2;
	reg p1;
	reg q1;
	reg p2;
	reg q2;
	reg cost_q1;
	reg cost_q2;
	reg cost_neq;
	reg root_neq;
	reg pred_neq;
	reg bright_neq;

   // wires to set the adders operators
   assign a1 = extern_data;
   assign b1 = ((pathfunction == 1'b0) || (state == ROOT_ST)) ? ~intern_data[0] : intern_data[0];
   assign pre_a2 = (pathfunction == 1'b0) ? extern_data : p1;
   assign a2 = (state == COST_ST) ? pre_a2 : direction;
   assign b2 = ~intern_data[1];
   
   // the carry in
   always@(posedge clk_i) begin
      if (state == STOP_ST) begin
         c1 <= intern_data[0];
         c2 <= intern_data[1];
      end
      else begin
         c1 <= q1 || root_carry_in;
         c2 <= q2 || root_carry_in;
      end
   end

   // adders
   always@* begin
      p1 <= (a1 ^ b1) ^ c1;
      p2 <= (a2 ^ b2) ^ c2;
      q1 <= (a1 & b1) | (c1 & (a1 ^ b1));
      q2 <= (a2 & b2) | (c2 & (a2 ^ b2));
   end

   // atribuation of result of computing operation
   always@(posedge clk_i) begin
      if (state == COST_ST) begin
         bright[7 : 0] <= { intern_data[0], bright[7 : 1] };
      end
      else if (state == SAVE_ST) begin
         bright[7 : 0] <= { result[8], bright[7 : 1] };
      end
      if (state != STOP_ST) begin
         result[22 : 0] <= result[23 : 1];
         result[26 : 24] <= result [27 : 25];
         if (state == COST_ST) begin
            if (pathfunction == 1'b0) result[23] <= extern_data;
            else result[23] <= p1;
         end
         else if (state == ROOT_ST) result[23] <= extern_data;
         else result[23] <= result[24];
         result[27] <= direction;
      end
   end

   // definition of query out for cost and root compare
   always@(posedge clk_i) begin
      if (state == COST_ST) begin
         cost_q1 <= q1;
         cost_q2 <= q2;
      end
      else if (state == STOP_ST) begin
         cost_q1 <= 1'b1;
         cost_q2 <= 1'b1;
      end
      if (state == STOP_ST)
			root_neq <= 1'b0;
      else if (state == ROOT_ST)
			root_neq <= root_neq | p1;
      if (state == STOP_ST)
			pred_neq <= 1'b0;
      else if (state == ROOT_ST)
			pred_neq <= pred_neq | p2;
      if (state == STOP_ST)
			cost_neq <= 1'b0;
      else if (state == COST_ST)
			cost_neq <= cost_neq | p2;
      if (state == STOP_ST)
			bright_neq <= 1'b0;
      else if (state == COST_ST)
			bright_neq <= bright_neq | (p1 ^ p2);
   end

   // the result and conquest
   always@* begin
      if ((cost_q1 == ~pathfunction) && ((cost_q2 == 1'b0) || ((cost_neq == 1'b0) && (root_neq == 1'b1) && (pred_neq == 1'b0)))) begin
         result_data <= result[0];
         conquest 	<= 1'b1;
      end
      else if ((pathfunction == 2'h0) && (cost_q2 == 1'b0) && ((bright_neq == 1'b1) || ((root_neq == 1'b1) && (pred_neq == 1'b0)))) begin
         result_data <= bright[0];
         conquest 	<= 1'b1;
      end
      else begin
         result_data <= intern_data[0];
         conquest 	<= 1'b0;
      end
   end

endmodule 
