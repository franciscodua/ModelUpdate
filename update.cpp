//
// Created by Francisco Duarte on 01/02/16.
//

#include "update.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_multifit.h>
#include <string>

int assingFunctionToSample(Sample sample, DBManager *dbManager) {
    int impactId;
    float error, minError;
    std::vector<ImpactFunction> impactFunctions;

    //NO IMPACT FUNCTION
    impactId = 1;
    minError = ERROR_THRESHOLD;

    impactFunctions = dbManager->getImpactFunctions();

    for (int i = 0; i < impactFunctions.size(); i++) {
        error = impactFunctions.at(i).computeError(sample);

        if (error < minError) {
            minError = error;
            impactId = impactFunctions.at(i).getImpactId();
        }
    }

    // set impactId that better represents sample
    sample.set_impactId(impactId);

    // adds sample to DB
    dbManager->addSample(sample);

    return impactId;
}

void updateFunction(int id, DBManager *dbManager) {
    std::vector<Sample> samples;
    gsl_matrix *X;
    gsl_vector *Y;
    gsl_vector *beta;
    size_t size;
    gsl_matrix *cov;

    double chisq;

    samples = dbManager->getSamples(id);

    size = samples.size();

    // size is number of rows and 2 is the number of variables (rspTime and resources)
    X = gsl_matrix_calloc(size, 3);
    Y= gsl_vector_alloc(size);
    beta = gsl_vector_alloc(3);

    // initialize vector Y and matrix X
    for (int i = 0; i < size; i++) {
        gsl_vector_set(Y, i, samples.at(i).getNewRspTime());

        gsl_matrix_set(X, i, 0, samples.at(i).getRspTime());
        gsl_matrix_set(X, i, 1, samples.at(i).getResources());
        // represents the constant part of the equation
        gsl_matrix_set(X, i, 2, 1);
    }

    cov = gsl_matrix_alloc(3, 3);

    gsl_multifit_linear_workspace *workspace = gsl_multifit_linear_alloc(size, 3);
    gsl_multifit_linear(X, Y, beta, cov, &chisq, workspace);

    printf("Beta:");
    for (int i = 0; i < 3; i++)
        printf("  %g", gsl_vector_get(beta, i));
    printf("\n");

    gsl_matrix_free(X);
    gsl_matrix_free(cov);
    gsl_vector_free(Y);
    gsl_vector_free(beta);
    gsl_multifit_linear_free(workspace);
}

void updateFunctions(DBManager *dbManager) {
    std::vector<int> ids;

    ids = dbManager->getImpactIds();

    for (int i = 0; i < ids.size(); i++) {
        updateFunction(ids.at(i), dbManager);
    }
}

void updateFunctions() {
    std::string cmd = "Rscript /Users/francisco/Documents/Working-dir/ModelUpdate/Plotting/UpdateModel.R";

    system(cmd.c_str());
}