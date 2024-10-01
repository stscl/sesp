#include <RcppArmadillo.h>
#include "Dummy.h"
using namespace Rcpp;
using namespace arma;

// [[Rcpp::depends(RcppArmadillo)]]


// [[Rcpp::export]]
double ComputeR2(const arma::vec& y, const arma::vec& y_pred) {
  // Calculate the mean of y
  double mean_y = arma::mean(y);

  // Calculate total variance (total sum of squares)
  double total_variance = arma::sum(arma::pow(y - mean_y, 2));

  // Calculate residual sum of squares (RSS)
  double residual_sum_of_squares = arma::sum(arma::pow(y - y_pred, 2));

  // Calculate R-squared
  double r_squared = 1 - (residual_sum_of_squares / total_variance);

  return r_squared;
}

// [[Rcpp::export]]
Rcpp::NumericVector SLMQ(const arma::mat& FitY, const arma::vec& Y) {
  // Get the number of columns in FitY
  int n_cols = FitY.n_cols;

  // Initialize a NumericVector to store R-squared values
  Rcpp::NumericVector r_squared_values(n_cols);

  // Loop through each column of FitY
  for (int i = 0; i < n_cols; ++i) {
    // Extract the i-th column of FitY
    arma::vec y_pred = FitY.col(i);

    // Compute R-squared for the current column
    double r2 = ComputeR2(Y, y_pred);

    // Store the R-squared value in the result vector
    r_squared_values[i] = r2;
  }

  // Return the vector of R-squared values
  return r_squared_values;
}

// transform each column of the input matrix into dummy variables, and then,
// for each column, retain only the corresponding dummy variables while setting
// all other dummy variables to zero.

// [[Rcpp::export]]
arma::mat PredictDummyY(const arma::imat& mat, const arma::vec& vec){
  int p = mat.n_cols; // Number of columns in the original matrix
  int n = mat.n_rows; // Number of rows in the original matrix

  // Generate the full dummy matrix
  arma::mat dummy_matrix = ArmaDummyMat(mat);

  // Initialize the result matrix
  arma::mat result(n, p, arma::fill::zeros);

  // Number of dummy variables per column
  std::vector<int> levels_count(p);
  int total_dummy_cols = 0;

  // Calculate the number of dummy variables per column
  for (int col_idx = 0; col_idx < p; ++col_idx) {
    arma::ivec levels = ArmaRunique(mat.col(col_idx));
    levels_count[col_idx] = levels.n_elem - 1;
    total_dummy_cols += levels.n_elem - 1;
  }

  // Iterate over each column of the original matrix
  int col_offset = 0;  // Offset to keep track of dummy variable positions
  for (int col_idx = 0; col_idx < p; ++col_idx) {
    // Copy the dummy matrix
    arma::mat modified_dummy = arma::zeros(n, total_dummy_cols);

    // Keep only the dummy variables for the current column, zero out others
    modified_dummy.cols(col_offset, col_offset + levels_count[col_idx]) = dummy_matrix.cols(col_offset, col_offset + levels_count[col_idx]);

    // Perform the multiplication with vec(1:p+1) and add vec(0) to the result
    result.col(col_idx) = modified_dummy * vec.subvec(1, p+1) + vec(0);

    // Update the offset for the next column
    col_offset += levels_count[col_idx];
  }

  return result; // Return the final result matrix
}

// calculating equivalent q-values under the framework of linear regression

// [[Rcpp::export]]
Rcpp::NumericVector CalculateQ(const arma::mat& y_pred,
                               const arma::imat& discmat,
                               const arma::vec& y) {
  int p = y_pred.n_cols;  // Number of columns (matches discmat)
  // int n = y_pred.n_rows;  // Number of rows (matches discmat and y)

  arma::vec q(p, fill::zeros);  // Initialize result vector

  // Iterate over each column of y_pred and discmat
  for (int col_idx = 0; col_idx < p; ++col_idx) {
    arma::ivec disc_col = discmat.col(col_idx); // Current column of discmat
    arma::ivec levels = ArmaRunique(disc_col);  // Get unique levels in this column

    // Squared differences for y_pred and y
    arma::vec total_diff = square(y_pred.col(col_idx) - y);
    double total_sum = sum(total_diff);  // Total sum of squared differences

    arma::vec group_sums(levels.n_elem, fill::zeros);  // Store group-wise sum of squared differences

    // Compute the sum of squared differences for each level
    int levelidx_length = levels.n_elem;
    for (int level_idx = 0; level_idx < levelidx_length; ++level_idx) {
      int current_level = levels(level_idx);

      // Mask for the current level
      arma::uvec mask = find(disc_col == current_level);

      // Sum the squared differences for this level
      group_sums(level_idx) = sum(total_diff.elem(mask));
    }

    // Compute q for this column as 1 - (group_sum / total_sum)
    double sum_of_ratios = sum(group_sums / total_sum);
    q(col_idx) = 1.0 - sum_of_ratios;
  }

  // Convert q to Rcpp::NumericVector and return
  return Rcpp::wrap(q);
}