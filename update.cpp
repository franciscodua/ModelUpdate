//
// Created by Francisco Duarte on 01/02/16.
//

#include "update.h"
#include <random>
#include <gsl/gsl_matrix.h>

#define N_REAL_SAMPLES 500

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
/*
void updateFunction(int id, DBManager *dbManager) {
    std::vector<Sample> samples;
    int size;

    std::string yStr;
    std::string matrixStr;

    samples = dbManager->getSamples(id);

    size = (int) samples.size();

    yStr = "[";
    matrixStr = "[";

    for (int i = 0; i < size; i++) {
        yStr += std::to_string(samples.at(i).getNewRspTime()) + ", ";

        matrixStr += "[" + std::to_string(samples.at(i).getRspTime()) +
                ", " + std::to_string(samples.at(i).getResources()) + "], ";
    }

    yStr.resize(yStr.size() - 1);
    matrixStr.resize(matrixStr.size() - 1);

    yStr += "]";
    matrixStr += "]";

    alglib::real_2d_array fmatrix = matrixStr.c_str();
    alglib::real_1d_array y = yStr.c_str();
    alglib::ae_int_t info;
    alglib::real_1d_array c;

    alglib::lsfitreport rep;


    alglib::lsfitlinear(y, fmatrix, info, c, rep);

    printf("c: %f", c.getcontent()[0]);
}*/

/*
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
*/
void updateFunctions(DBManager *dbManager) {
    std::vector<int> ids;

    ids = dbManager->getImpactIds();

    for (int i = 0; i < ids.size(); i++) {
        //updateFunction(ids.at(i), dbManager);
    }
}

void updateFunctions() {
    std::string cmd = "Rscript /Users/francisco/Documents/IST/Working-dir/ModelUpdate/Plotting/UpdateModel.R";

    system(cmd.c_str());
}

void generateRealSamples(ImpactFunction impact, DBManager *dbManager) {
    int nDimensions;
    std::vector<float> vars(2);
    std::vector<int> minRanges(2);
    std::vector<int> maxRanges(2);
    Sample *sample;
    int samples;
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0,1.0);
    float randomNumber;
    float rspTime, resources;

    nDimensions = impact.getNDimensions();
    // number of samples to be generated. Probability times constant
    samples = N_REAL_SAMPLES;

    for (int i = 0; i < nDimensions - 1; i++) {
        minRanges[i] = (int) impact.getMinRange(i);
        maxRanges[i] = (int) impact.getMaxRange(i);
    }

    sample = new Sample(0, 0, 0, false);

    // generates samples.
    for (int i = 0; i < samples; i++) {
        randomNumber = distribution(generator);
        rspTime = (randomNumber * (maxRanges[0] - minRanges[0])) + minRanges[0];
        vars[0] = rspTime;

        randomNumber = distribution(generator);
        resources = (randomNumber * (maxRanges[1] - minRanges[1])) + minRanges[1];
        vars[1] = resources;

        sample->setRspTime(rspTime);
        sample->setResources(resources);
        sample->setNewRspTime(impact.computeOutput(vars));

        assingFunctionToSample(*sample, dbManager);
    }

    delete(sample);
}
