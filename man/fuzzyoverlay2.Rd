% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/fuzzyoverlay2.R
\name{fuzzyoverlay2}
\alias{fuzzyoverlay2}
\title{Spatial fuzzy overlay between variables pairwise}
\usage{
fuzzyoverlay2(formula, data, method = "and")
}
\arguments{
\item{formula}{A formula.}

\item{data}{A \code{data.frame} or \code{tibble} of discretized data.}

\item{method}{(optional) Spatial overlay method. One of \code{and}, \code{or}, \code{intersection}.
Default is \code{and}.}
}
\value{
A list
\describe{
\item{\code{overlay}}{overlay results between pairs of variables}
\item{\code{variable}}{pairwise interacting variable}
}
}
\description{
Spatial fuzzy overlay between variables pairwise
}
\examples{
sim = tibble::tibble(y = stats::runif(7,0,10),
                     x1 = c(1,rep(2,3),rep(3,3)),
                     x2 = c(rep(1,2),rep(2,2),rep(3,3)),
                     x3 = c(rep(1,3),rep(2,2),rep(3,2)))
fo1 = fuzzyoverlay2(y ~ .,data = sim, method = 'and')
fo1
fo2 = fuzzyoverlay2(y ~ .,data = sim, method = 'or')
fo2
fo3 = fuzzyoverlay2(y ~ .,data = sim, method = 'intersection')
fo3

}
