import ProvenHashes
#check @ProvenHashes.uniformProb_mono
#print axioms ProvenHashes.uniformProb_mono
#check @ProvenHashes.uniformProb_or_le
#print axioms ProvenHashes.uniformProb_or_le
#check @ProvenHashes.uniformProb_const
#print axioms ProvenHashes.uniformProb_const
#check @ProvenHashes.uniformProb_prod
#print axioms ProvenHashes.uniformProb_prod
#check @ProvenHashes.uniformProb_prod_fst
#print axioms ProvenHashes.uniformProb_prod_fst
#check @ProvenHashes.uniformProb_prod_le
#print axioms ProvenHashes.uniformProb_prod_le
#check @ProvenHashes.compose_collision_bound
#print axioms ProvenHashes.compose_collision_bound
#check @ProvenHashes.chainhash_equal_length_from_stages
#print axioms ProvenHashes.chainhash_equal_length_from_stages
#check @ProvenHashes.chainhash_different_lengths_from_stages
#print axioms ProvenHashes.chainhash_different_lengths_from_stages
#check @ProvenHashes.Recurrence.seed_monic
#print axioms ProvenHashes.Recurrence.seed_monic
#check @ProvenHashes.Recurrence.seed_degree
#print axioms ProvenHashes.Recurrence.seed_degree
#check @ProvenHashes.Recurrence.seed_top
#print axioms ProvenHashes.Recurrence.seed_top
#check @ProvenHashes.Recurrence.data_coeff_zero
#print axioms ProvenHashes.Recurrence.data_coeff_zero
#check @ProvenHashes.Recurrence.shift_coeff_zero
#print axioms ProvenHashes.Recurrence.shift_coeff_zero
#check @ProvenHashes.Recurrence.shift_top
#print axioms ProvenHashes.Recurrence.shift_top
#check @ProvenHashes.Recurrence.head_a
#print axioms ProvenHashes.Recurrence.head_a
#check @ProvenHashes.Recurrence.head_b
#print axioms ProvenHashes.Recurrence.head_b
#check @ProvenHashes.Recurrence.peel_encode
#print axioms ProvenHashes.Recurrence.peel_encode
#check @ProvenHashes.Recurrence.decode_encode
#print axioms ProvenHashes.Recurrence.decode_encode
#check @ProvenHashes.Recurrence.encode_injective
#print axioms ProvenHashes.Recurrence.encode_injective
#check @ProvenHashes.same_bucket_close
#print axioms ProvenHashes.same_bucket_close
#check @ProvenHashes.multiplyShift_collision_close
#print axioms ProvenHashes.multiplyShift_collision_close
#check @ProvenHashes.odd_mul_mod_injective
#print axioms ProvenHashes.odd_mul_mod_injective
#check @ProvenHashes.affine_bijective
#print axioms ProvenHashes.affine_bijective
#check @ProvenHashes.sum_mul_update
#print axioms ProvenHashes.sum_mul_update
#check @ProvenHashes.affine_sum_uniform
#print axioms ProvenHashes.affine_sum_uniform
#check @ProvenHashes.nh_difference
#print axioms ProvenHashes.nh_difference
#check @ProvenHashes.nh_difference_uniform
#print axioms ProvenHashes.nh_difference_uniform
#check @ProvenHashes.nh_collision_bound
#print axioms ProvenHashes.nh_collision_bound
#check @ProvenHashes.polynomialHash_eq_eval
#print axioms ProvenHashes.polynomialHash_eq_eval
#check @ProvenHashes.polynomial_zero_count
#print axioms ProvenHashes.polynomial_zero_count
#check @ProvenHashes.polynomial_collision_bound
#print axioms ProvenHashes.polynomial_collision_bound
#check @ProvenHashes.polynomial_collision_zmod
#print axioms ProvenHashes.polynomial_collision_zmod
#check @ProvenHashes.gf64_card
#print axioms ProvenHashes.gf64_card
#check @ProvenHashes.polynomial_collision_gf64
#print axioms ProvenHashes.polynomial_collision_gf64
#check @ProvenHashes.uniformProb_equiv
#print axioms ProvenHashes.uniformProb_equiv
#check @ProvenHashes.uniformProb_fst
#print axioms ProvenHashes.uniformProb_fst
#check @ProvenHashes.uniformProb_of_bijective_slices
#print axioms ProvenHashes.uniformProb_of_bijective_slices
#check @ProvenHashes.uniformProb_of_bijective_update
#print axioms ProvenHashes.uniformProb_of_bijective_update
#check @ProvenHashes.Recurrence.slice_lift
#print axioms ProvenHashes.Recurrence.slice_lift
#check @ProvenHashes.Recurrence.slice_X
#print axioms ProvenHashes.Recurrence.slice_X
#check @ProvenHashes.Recurrence.extract_keyPolynomial
#print axioms ProvenHashes.Recurrence.extract_keyPolynomial
#check @ProvenHashes.Recurrence.decode_keyPolynomial
#print axioms ProvenHashes.Recurrence.decode_keyPolynomial
#check @ProvenHashes.Recurrence.keyPolynomial_injective
#print axioms ProvenHashes.Recurrence.keyPolynomial_injective
#check @ProvenHashes.Recurrence.keyCoefficients_injective
#print axioms ProvenHashes.Recurrence.keyCoefficients_injective
#check @ProvenHashes.Recurrence.fold_expansion
#print axioms ProvenHashes.Recurrence.fold_expansion
#check @ProvenHashes.Recurrence.eval_lift
#print axioms ProvenHashes.Recurrence.eval_lift
#check @ProvenHashes.Recurrence.eval_keyPolynomial
#print axioms ProvenHashes.Recurrence.eval_keyPolynomial
#check @ProvenHashes.Recurrence.lift_degree
#print axioms ProvenHashes.Recurrence.lift_degree
#check @ProvenHashes.Recurrence.component_difference_degrees
#print axioms ProvenHashes.Recurrence.component_difference_degrees
#check @ProvenHashes.Recurrence.keyPolynomial_difference_degree
#print axioms ProvenHashes.Recurrence.keyPolynomial_difference_degree
#check @ProvenHashes.Recurrence.collision_bound
#print axioms ProvenHashes.Recurrence.collision_bound
#check @ProvenHashes.Recurrence.collision_bound_gf64
#print axioms ProvenHashes.Recurrence.collision_bound_gf64
#check @ProvenHashes.Recurrence.keyPolynomial_degree
#print axioms ProvenHashes.Recurrence.keyPolynomial_degree
#check @ProvenHashes.Recurrence.collision_bound_any_length
#print axioms ProvenHashes.Recurrence.collision_bound_any_length
#check @ProvenHashes.tabulation_difference_uniform
#print axioms ProvenHashes.tabulation_difference_uniform
#check @ProvenHashes.tabulation_collision_exact
#print axioms ProvenHashes.tabulation_collision_exact

-- BRW lane additions
#check @ProvenHashes.BRW.split_injective
#print axioms ProvenHashes.BRW.split_injective
#check @ProvenHashes.BRW.tree_shape
#print axioms ProvenHashes.BRW.tree_shape
#check @ProvenHashes.BRW.tree_injective
#print axioms ProvenHashes.BRW.tree_injective
#check @ProvenHashes.BRW.tree_degree
#print axioms ProvenHashes.BRW.tree_degree
#check @ProvenHashes.BRW.tree_stable
#print axioms ProvenHashes.BRW.tree_stable
#check @ProvenHashes.BRW.tree_eq_polynomial
#print axioms ProvenHashes.BRW.tree_eq_polynomial
#check @ProvenHashes.BRW.polynomial_recursion
#print axioms ProvenHashes.BRW.polynomial_recursion
#check @ProvenHashes.BRW.polynomial_base
#print axioms ProvenHashes.BRW.polynomial_base
#check @ProvenHashes.BRW.encode_injective
#print axioms ProvenHashes.BRW.encode_injective
#check @ProvenHashes.BRW.encode_degree
#print axioms ProvenHashes.BRW.encode_degree
#check @ProvenHashes.BRW.collision_bound
#print axioms ProvenHashes.BRW.collision_bound
#check @ProvenHashes.BRW.collision_bound_gf64
#print axioms ProvenHashes.BRW.collision_bound_gf64
#check @ProvenHashes.EightLanes.collision_bound
#print axioms ProvenHashes.EightLanes.collision_bound
#check @ProvenHashes.EightLanes.lanes_length
#print axioms ProvenHashes.EightLanes.lanes_length
#check @ProvenHashes.EightLanes.lanes_injective
#print axioms ProvenHashes.EightLanes.lanes_injective
#check @ProvenHashes.EightLanes.word_collision_bound_gf64
#print axioms ProvenHashes.EightLanes.word_collision_bound_gf64
#check @ProvenHashes.ChartScores.brw_ratio_gt
#print axioms ProvenHashes.ChartScores.brw_ratio_gt
#check @ProvenHashes.ChartScores.brw_ratio_antitone
#print axioms ProvenHashes.ChartScores.brw_ratio_antitone
#check @ProvenHashes.ChartScores.brw_minimum
#print axioms ProvenHashes.ChartScores.brw_minimum
#check @ProvenHashes.ChartScores.brw_minimum_bracket
#print axioms ProvenHashes.ChartScores.brw_minimum_bracket
#check @ProvenHashes.ChartScores.lane_ratio_ge
#print axioms ProvenHashes.ChartScores.lane_ratio_ge
#check @ProvenHashes.ChartScores.lane_minimum
#print axioms ProvenHashes.ChartScores.lane_minimum
#check @ProvenHashes.ChartScores.brw_whole_bits
#print axioms ProvenHashes.ChartScores.brw_whole_bits
#check @ProvenHashes.ChartScores.score_formulas
#print axioms ProvenHashes.ChartScores.score_formulas
#check @ProvenHashes.ChartScores.probability_floor
#print axioms ProvenHashes.ChartScores.probability_floor
