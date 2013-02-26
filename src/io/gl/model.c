/**
 * model.c
 * clockwork
 * February 26, 2013
 * Brandon Surmanski
 */

void model_init(Model *model)
{
    model->ncomponents = 0;
    model->components = NULL;
}

void model_finalize(Model *model)
{
    int i;
    for(i = 0; i < model->ncomponents; i++)
    {
        //TODO: finalize component
    }
    free(model->components);

    model->ncomponents = 0;
    model->components = NULL;
}
