#include "flim.h"

FLIM::FLIM()
{
    this->orig_dir = "";
    this->param_dir = "";
    this->seeds_dir = "";
    this->object_dir = "";
    this->images_list_path = "";


    this->device = -1;
    ift_arch = nullptr;
}

FLIM::~FLIM()
{
    iftDestroyFLIMArch(&this->ift_arch);
}

bool FLIM::IsArchitectureValid()
{
    /* Standard global keys */
    // stdev_factor
    QMessageBox msgbox;
    if (!this->arch_json.contains("stdev_factor")){
        QErrorMessage * message = new QErrorMessage();
        message->showMessage(QString("Failed to load architecture. Key \"stdev_factor\" not found"));
        delete message;
        msgbox.setText(QString("Failed to load architecture. Key \"stdev_factor\" not found"));
        msgbox.exec();
        return false;
    } else {
        if (!this->arch_json.value(QString("stdev_factor")).isDouble()){
            QErrorMessage * message = new QErrorMessage();
            message->showMessage(QString("Stdev_factor must be a float point value."));
            delete message;
            msgbox.setText(QString("Stdev_factor must be a float point value."));
            msgbox.exec();
            return false;
        }
    }
    // nlayers
    if (!this->arch_json.contains("nlayers")){
        QErrorMessage * message = new QErrorMessage();
        message->showMessage(QString("Failed to load architecture. Key \"nlayers\" not found"));
        delete message;
        msgbox.setText(QString("Failed to load architecture. Key \"nlayers\" not found"));
        msgbox.exec();
        return false;
    } else {
        if (this->arch_json.value(QString("nlayers")).toInt(-1) == -1){
            QErrorMessage * message = new QErrorMessage();
            message->showMessage(QString("Number of layers must be a positive integer."));
            delete message;
            msgbox.setText(QString("Number of layers must be a positive integer."));
            msgbox.exec();
            return false;
        }
    }

    /* Checking layer key structures */
    int nlayers = this->arch_json.value("nlayers").toInt();
    QErrorMessage * message = new QErrorMessage();
    for (int l = 0; l < nlayers; l++){
        if (!this->arch_json.contains(QString("layer%1").arg(l+1))){
            message->showMessage(QString("Layer %1 missing.").arg(l+1));
            delete message;
            msgbox.setText(QString("Layer %1 missing.").arg(l+1));
            msgbox.exec();
            return false;
        }

        QJsonObject jsonLayer = this->arch_json[QString("layer%1").arg(l+1)].toObject();
        if (!jsonLayer.contains(QString("conv"))){
            message->showMessage(QString("Layer %1 missing \"conv\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Layer %1 missing \"conv\" key.").arg(l+1));
            msgbox.exec();
            return false;
        }
        if (!jsonLayer.contains(QString("relu"))){
            message->showMessage(QString("Layer %1 missing \"relu\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Layer %1 missing \"relu\" key.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (!jsonLayer.value(QString("relu")).isBool()){
                message->showMessage(QString("Relu key of Layer %1 must be boolean.").arg(l+1));
                delete message;
                msgbox.setText(QString("Relu key of Layer %1 must be boolean.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }
        if (!jsonLayer.contains(QString("pooling"))){
            message->showMessage(QString("Layer %1 missing \"pooling\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Layer %1 missing \"pooling\" key.").arg(l+1));
            msgbox.exec();
            return false;
        }

        QJsonObject jsonConv = jsonLayer.value(QString("conv")).toObject();
        if (!jsonConv.contains(QString("kernel_size"))){
            message->showMessage(QString("Conv structure of layer %1 missing \"kernel_size\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Conv structure of layer %1 missing \"kernel_size\" key.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (!jsonConv.value(QString("kernel_size")).isArray()){
                message->showMessage(QString("Kernel size key of Layer %1 must be an array.").arg(l+1));
                delete message;
                msgbox.setText(QString("Kernel size key of Layer %1 must be an array.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }
        if (!jsonConv.contains(QString("nkernels_per_marker"))){
            message->showMessage(QString("Conv structure of layer %1 missing \"nkernels_per_marker\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Conv structure of layer %1 missing \"nkernels_per_marker\" key.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (jsonConv.value(QString("nkernels_per_marker")).toInt(-1) == -1){
                message->showMessage(QString("The nkernels_per_marker key of Layer %1 must be a positive integer.").arg(l+1));
                delete message;
                msgbox.setText(QString("The nkernels_per_marker key of Layer %1 must be a positive integer.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }
        if (!jsonConv.contains(QString("dilation_rate"))){
            message->showMessage(QString("Conv structure of layer %1 missing \"dilation_rate\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Conv structure of layer %1 missing \"dilation_rate\" key.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (!jsonConv.value(QString("dilation_rate")).isArray()){
                message->showMessage(QString("The dilation_rate key of Layer %1 must be an array.").arg(l+1));
                delete message;
                msgbox.setText(QString("The dilation_rate key of Layer %1 must be an array.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }
        if (!jsonConv.contains(QString("nkernels_per_image"))){
            message->showMessage(QString("Conv structure of layer %1 missing \"nkernels_per_image\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Conv structure of layer %1 missing \"nkernels_per_image\" key.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (jsonConv.value(QString("nkernels_per_image")).toInt(-1) == -1){
                message->showMessage(QString("The nkernels_per_image key of Layer %1 must be a positive integer.").arg(l+1));
                delete message;
                msgbox.setText(QString("The nkernels_per_image key of Layer %1 must be a positive integer.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }
        if (!jsonConv.contains(QString("noutput_channels"))){
            message->showMessage(QString("Conv structure of layer %1 missing \"noutput_channels\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Conv structure of layer %1 missing \"noutput_channels\" key.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (jsonConv.value(QString("noutput_channels")).toInt(-1) == -1){
                message->showMessage(QString("The noutput_channels key of Layer %1 must be a positive integer.").arg(l+1));
                delete message;
                msgbox.setText(QString("The noutput_channels key of Layer %1 must be a positive integer.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }

        QJsonObject jsonPooling = jsonLayer.value(QString("pooling")).toObject();
        if (!jsonPooling.contains(QString("type"))){
            message->showMessage(QString("Pooling structure of layer %1 missing \"type\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Pooling structure of layer %1 missing \"type\" key.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (!jsonPooling.value(QString("type")).isString()){
                message->showMessage(QString("The type key of Layer %1 must be a string.").arg(l+1));
                delete message;
                msgbox.setText(QString("The type key of Layer %1 must be a string.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }
        if (!jsonPooling.contains(QString("size"))){
            message->showMessage(QString("Pooling structure of layer %1 missing \"size\" key.").arg(l+1));
            delete message;
            msgbox.setText(QString("Pooling structure of layer %1 missing \"size\" key.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (!jsonPooling.value(QString("size")).isArray()){
                message->showMessage(QString("Pooling size key of Layer %1 must be an array.").arg(l+1));
                delete message;
                msgbox.setText(QString("Pooling size key of Layer %1 must be an array.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }
        if (!jsonPooling.contains(QString("stride"))){
            message->showMessage(QString("Stride key of Layer %1 must be an array.").arg(l+1));
            delete message;
            msgbox.setText(QString("Stride key of Layer %1 must be an array.").arg(l+1));
            msgbox.exec();
            return false;
        } else {
            if (jsonPooling.value(QString("stride")).toInt(-1) == -1){
                message->showMessage(QString("The stride key of Layer %1 must be a positive integer.").arg(l+1));
                delete message;
                msgbox.setText(QString("The stride key of Layer %1 must be a positive integer.").arg(l+1));
                msgbox.exec();
                return false;
            }
        }
    }

    delete message;
    return true;
}

bool FLIM::modelExists(int layer)
{
    QDir model_dir(this->param_dir);
    QFileInfoList list = model_dir.entryInfoList(QStringList() << "LayerParams" << QString("conv%1-kernels.npy").arg(layer),QDir::Files);
    if (list.count() == 0){
        return FALSE;
    } else {
        return TRUE;
    }
}


bool FLIM::TrainLayerFLIM(int layer)
{
    // Setting layer input as previous layer output
    QString layer_input;
    QString layer_output;
    QString layer_image_list;
    bool done = TRUE;

    if (layer == 1){
        layer_input = this->orig_dir;
        layer_output = this->param_dir+"/layer"+QVariant(layer).toString()+"/";
    }else{
        QDir weight_dir(this->param_dir);
        QFileInfoList weight_list = weight_dir.entryInfoList(QStringList() << "Weights" << QString("conv%1-*").arg(layer-1),QDir::Files);
        if (weight_list.empty()){ // previous layer has no weights
            done = this->TrainLayerFLIM(layer-1);
        }
        if (!done){
            return FALSE;
        }

        layer_input = this->param_dir+"/layer"+QVariant(layer-1).toString()+"/";
        layer_output = this->param_dir+"/layer"+QVariant(layer).toString()+"/";
    }

    if (iftDirExists(layer_output.toUtf8().data())){
        iftRemoveDir(layer_output.toUtf8().data());
        iftMakeDir(layer_output.toUtf8().data());
    } else {
        iftMakeDir(layer_output.toUtf8().data());
    }

    // Creating architecture file for layer
    QJsonObject json_layer;
    json_layer.insert("stdev_factor",this->arch_json.value(QString("stdev_factor")));
    json_layer.insert("nlayers",1);
    json_layer.insert("apply_intrinsic_atrous",this->arch_json.value(QString("apply_intrinsic_atrous")));
    json_layer.insert("layer1",this->arch_json.value(QString("layer")+QVariant(layer).toString()));
    QJsonDocument *save = new QJsonDocument(json_layer);
    QFile *save_arch = new QFile(this->param_dir+QString("arch_layer%1.json").arg(layer));
    if (!save_arch->open(QIODevice::WriteOnly)){
        qWarning("Could not open arch file");
        return FALSE;
    }
    save_arch->write(save->toJson());
    save_arch->close();

    // Reading architecture
    iftFLIMArch *tmp_arch = iftReadFLIMArch((this->param_dir+QString("arch_layer%1.json").arg(layer)).toUtf8().data());

    iftFLIMLearnLayer(layer_input.toUtf8().data(),this->seeds_dir.toUtf8().data(), this->param_dir.toUtf8().data(),layer,tmp_arch,layer_output.toUtf8().data());

    /* Writting architecture in case it changed */
    delete save;
    delete save_arch;
    this->arch_path = this->param_dir + "/arch.json";
    this->arch_json[QString("layer%1").arg(layer)].toObject()["noutput_channels"] = tmp_arch->layer[layer].noutput_channels;
    save = new QJsonDocument(this->arch_json);
    save_arch = new QFile(this->arch_path);
    if (!save_arch->open(QIODevice::WriteOnly)){
        qWarning("Could not open arch file");
        return FALSE;
    }
    save_arch->write(save->toJson());
    save_arch->close();

    iftDestroyFLIMArch(&tmp_arch);

    return TRUE;
}

bool FLIM::ExtractFeaturesFromLayer(int layer)
{

    // Checking if model exists
    QDir weight_dir(this->param_dir);
    QFileInfoList weight_list = weight_dir.entryInfoList(QStringList() << "Weights" << QString("conv%1-*").arg(layer),QDir::Files);
    if (weight_list.empty()){ // previous layer has no weights
        this->TrainLayerFLIM(layer);
    }

    // Setting layer input as previous layer output
    QString layer_input;
    QString layer_image_list;
    if (layer == 1){
        layer_input = this->orig_dir;
        layer_image_list = this->param_dir+"input.csv";
    }else{
        layer_input = this->param_dir+"activation"+QVariant(layer-1).toString()+"/";
        layer_image_list = this->param_dir+"activation"+QVariant(layer-1).toString()+".csv";
    }

    QDir dir(layer_input);
    QFileInfoList list = dir.entryInfoList(QStringList() << "Activations" << "*",QDir::Files);
    QDir image_dir(this->orig_dir);
    QFileInfoList list_orig = image_dir.entryInfoList(QStringList() << "Images" << "*", QDir::Files);
    if (list.count() < list_orig.count()){
        this->ExtractFeaturesFromLayer(layer-1);
        dir = QDir(layer_input);
        list = dir.entryInfoList(QStringList() << "Activations" << "*",QDir::Files); //update list to get new activations
    }

    // Creating list of images
    QFile images_csv(layer_image_list);
    if (images_csv.open(QIODevice::WriteOnly)){
        foreach(QFileInfo filename, list) {
            images_csv.write((filename.fileName()+"\n").toUtf8().data());
        }
    }
    images_csv.close();

    // I have to create a char pointer to the object dir due to the fact that the iftFLIMExtractFeatures function checks if the object dir is null. If, an empty string ("") is
    // sent as argument, it generates a bug.
    char *object_dir_ptr = nullptr;
    if (this->object_dir != ""){
        object_dir_ptr = iftCopyString(this->object_dir.toUtf8().data());
    }

    // Creating output dir for layer
    QString activation_dir =  this->param_dir+"activation"+QVariant(layer).toString()+"/";
    if (iftDirExists(activation_dir.toUtf8().data())){
        iftRemoveDir(activation_dir.toUtf8().data());
        iftMakeDir(activation_dir.toUtf8().data());
    } else {
        iftMakeDir(activation_dir.toUtf8().data());
    }

    // Creating architecture file for layer
    QJsonObject *json_layer = new QJsonObject();
    json_layer->insert("stdev_factor",this->arch_json.value(QString("stdev_factor")));
    json_layer->insert("nlayers",1);
    json_layer->insert("layer1",this->arch_json.value(QString("layer")+QVariant(layer).toString()));
    QJsonDocument save(*json_layer);
    QFile save_arch(this->param_dir+QString("arch_layer%1.json").arg(layer));
    if (!save_arch.open(QIODevice::WriteOnly)){
        qWarning("Could not open arch file");
        return FALSE;
    }
    save_arch.write(save.toJson());
    save_arch.close();
    delete json_layer;

    // Reading architecture
    iftFLIMArch *tmp_arch = iftReadFLIMArch((this->param_dir+QString("arch_layer%1.json").arg(layer)).toUtf8().data());

    // Extracting Features
    iftFLIMExtractFeaturesFromLayer(layer_input.toUtf8().data(),layer_image_list.toUtf8().data(),tmp_arch,this->param_dir.toUtf8().data(),layer,activation_dir.toUtf8().data(),object_dir_ptr,this->device);
    iftDestroyFLIMArch(&tmp_arch);

    if (object_dir_ptr != nullptr)
        free(object_dir_ptr);
    
    /* Verify if layer<n> exists, which is necessary to visualize activations */
    QString layer_output;
    layer_output = this->param_dir+"/layer"+QVariant(layer).toString()+"/";
    if (!iftDirExists(layer_output.toUtf8().data())){
        iftMakeDir(layer_output.toUtf8().data());
    }

    return TRUE;
}

bool FLIM::ExtractFeaturesFromLayerForTraining(int layer)
{

    // Checking if model exists
    QDir weight_dir(this->param_dir);
    QFileInfoList weight_list = weight_dir.entryInfoList(QStringList() << "Weights" << QString("conv%1-*").arg(layer),QDir::Files);
    if (weight_list.empty()){ // previous layer has no weights
        this->TrainLayerFLIM(layer);
    }

    // Setting layer input as previous layer output
    QString layer_input;
    QString layer_image_list;
    if (layer == 1){
        layer_input = this->orig_dir;
        layer_image_list = this->param_dir+"input.csv";
    }else{
        layer_input = this->param_dir+"layer"+QVariant(layer-1).toString()+"/";
        layer_image_list = this->param_dir+"layer"+QVariant(layer-1).toString()+".csv";
    }

    QDir dir(layer_input);
    QFileInfoList list = dir.entryInfoList(QStringList() << "Activations" << "*",QDir::Files);
    QDir image_dir(this->orig_dir);
    QFileInfoList list_orig = image_dir.entryInfoList(QStringList() << "Images" << "*", QDir::Files);
    if (list.count() < list_orig.count()){
        this->ExtractFeaturesFromLayer(layer-1);
        dir = QDir(layer_input);
        list = dir.entryInfoList(QStringList() << "Activations" << "*",QDir::Files); //update list to get new activations
    }

    // Creating list of images
    QFile images_csv(layer_image_list);
    if (images_csv.open(QIODevice::WriteOnly)){
        foreach(QFileInfo filename, list) {
            images_csv.write((filename.fileName()+"\n").toUtf8().data());
        }
    }
    images_csv.close();

    // I have to create a char pointer to the object dir due to the fact that the iftFLIMExtractFeatures function checks if the object dir is null. If, an empty string ("") is
    // sent as argument, it generates a bug.
    char *object_dir_ptr = nullptr;
    if (this->object_dir != ""){
        object_dir_ptr = iftCopyString(this->object_dir.toUtf8().data());
    }

    // Creating output dir for layer
    QString activation_dir =  this->param_dir+"layer"+QVariant(layer).toString()+"/";
    if (iftDirExists(activation_dir.toUtf8().data())){
        iftRemoveDir(activation_dir.toUtf8().data());
        iftMakeDir(activation_dir.toUtf8().data());
    } else {
        iftMakeDir(activation_dir.toUtf8().data());
    }

    // Creating architecture file for layer
    QJsonObject *json_layer = new QJsonObject();
    json_layer->insert("stdev_factor",this->arch_json.value(QString("stdev_factor")));
    json_layer->insert("nlayers",1);
    json_layer->insert("layer1",this->arch_json.value(QString("layer")+QVariant(layer).toString()));
    QJsonDocument save(*json_layer);
    QFile save_arch(this->param_dir+QString("arch_layer%1.json").arg(layer));
    if (!save_arch.open(QIODevice::WriteOnly)){
        qWarning("Could not open arch file");
        return FALSE;
    }
    save_arch.write(save.toJson());
    save_arch.close();
    delete json_layer;

    // Reading architecture
    iftFLIMArch *tmp_arch = iftReadFLIMArch((this->param_dir+QString("arch_layer%1.json").arg(layer)).toUtf8().data());
    // Changing pooling stride to 1
    tmp_arch->layer[0].pool_stride = 1;

    // Extracting Features
    iftFLIMExtractFeaturesFromLayer(layer_input.toUtf8().data(),layer_image_list.toUtf8().data(),tmp_arch,this->param_dir.toUtf8().data(),layer,activation_dir.toUtf8().data(),object_dir_ptr,this->device);
    iftDestroyFLIMArch(&tmp_arch);

    if (object_dir_ptr != nullptr)
        free(object_dir_ptr);

    /* Verify if layer<n> exists, which is necessary to visualize activations */
    QString layer_output;
    layer_output = this->param_dir+"/layer"+QVariant(layer).toString()+"/";
    if (!iftDirExists(layer_output.toUtf8().data())){
        iftMakeDir(layer_output.toUtf8().data());
    }

    return TRUE;
}

QString FLIM::ExtractFeaturesWithPython()
{
    this->arch_path = this->param_dir + "/arch.json";

    /* Saving arch.json so we can read it as an IFT structure */
    QJsonDocument save(this->arch_json);
    QFile save_arch(this->arch_path);
    if (!save_arch.open(QIODevice::WriteOnly)){
        qWarning("Could not open arch file");
        return "Could not open arch file";
    }
    save_arch.write(save.toJson());
    save_arch.close();

    /* Training */
    //QStringList arguments { "feature_extraction.py", "-i "+this->orig_dir, "-a "+this->arch_path, "-o "+this->param_dir};
    //qDebug() << arguments;
    QString script_path = QCoreApplication::applicationDirPath() + "../FLIMbuilder/feature_extraction";
    QStringList arguments {script_path, " -i "+this->orig_dir, " -a "+this->arch_path, " -o "+this->param_dir};
    QProcess p;
    p.start("python", arguments);
    qDebug() << p.errorString();
    qDebug() << p.error();
    p.waitForFinished();

    return p.readAllStandardOutput();
}

iftFLIMArch *JSONArchToFLIMArch(QJsonObject arch_json)
{
    iftFLIMArch *flim_arch = static_cast<iftFLIMArch *>(calloc(1, sizeof(iftFLIMArch)));

    flim_arch->nlayers = arch_json.value(QString("nlayers")).toInt();
    flim_arch->stdev_factor = arch_json.value(QString("stdev_factor")).toInt();

    flim_arch->layer = static_cast<iftFLIMLayer *>(calloc(ulong(flim_arch->nlayers), sizeof(iftFLIMLayer)));

    for (int l = 0; l < flim_arch->nlayers; l++){
        QJsonObject jsonLayer = arch_json[QString("layer%1").arg(l+1)].toObject();
        QJsonObject jsonConv = jsonLayer.value(QString("conv")).toObject();
        QJsonArray kernel_size = jsonConv.value(QString("kernel_size")).toArray();
        QJsonArray dilation_rate = jsonConv.value(QString("kernel_size")).toArray();
        int nkernels_per_markers = jsonConv.value(QString("nkernels_per_marker")).toInt();
        int nkernels_per_image   = jsonConv.value(QString("nkernels_per_image")).toInt();
        int noutput_channels     = jsonConv.value(QString("noutput_channels")).toInt();

        bool relu                = jsonLayer.value(QString("relu")).toBool();

        QJsonObject jsonPooling  = jsonLayer.value(QString("pooling")).toObject();
        QJsonArray pool_size     = jsonPooling.value(QString("size")).toArray();
        QString type             = jsonPooling.value(QString("type")).toString();
        int pool_stride          = jsonPooling.value(QString("stride")).toInt();

        for (int i = 0; i < 3; i++){
            flim_arch->layer[l].kernel_size[i]   = kernel_size.at(i).toInt();
            flim_arch->layer[l].dilation_rate[i] = dilation_rate.at(i).toInt();
            flim_arch->layer[l].pool_size[i]     = pool_size.at(i).toInt();
        }
        flim_arch->layer[l].nkernels_per_marker = nkernels_per_markers;
        flim_arch->layer[l].nkernels_per_image  = nkernels_per_image;
        flim_arch->layer[l].noutput_channels    = noutput_channels;

        flim_arch->layer[l].relu = relu;

        flim_arch->layer[l].pool_type = type.toUtf8().data();

        flim_arch->layer[l].pool_stride = pool_stride;
    }

    return flim_arch;
}

void StatisticsFromAllSeeds(iftFileSet *fs_seeds, char *inputdata_dir, iftAdjRel *A, float *mean, float *stdev, float stdev_factor, char *ext) {
    int nseeds = 0;
    long ninput_channels = 0;
    char *basename = nullptr;
    char filename[200];
    iftMImage *input = nullptr;

    for (int i = 0; i < fs_seeds->n; i++) {
        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        if (strcmp(ext,"mimg") == 0){
            sprintf(filename, "%s/%s.mimg", inputdata_dir, basename);
            input = iftReadMImage(filename);
        } else {
            sprintf(filename, "%s/%s.%s", inputdata_dir, basename, ext);
            input = ReadInputMImage(filename);
        }
        iftFree(basename);

        ninput_channels = input->m;
        iftLabeledSet *S = iftMReadSeeds(input, fs_seeds->files[i]->path);
        while (S != nullptr) {
            int l;
            int p = iftRemoveLabeledSet(&S, &l);

            /*nseeds += 1;
            for (int b = 0; b < ninput_channels; b++) {
                mean[b] += input->val[p][b];
            }*/

            iftVoxel u = iftMGetVoxelCoord(input,p);
            for (int a = 0; a < A->n; a++) {
                iftVoxel v = iftGetAdjacentVoxel(A,u,a);
                if (iftMValidVoxel(input,v)) {
                    int q = iftMGetVoxelIndex(input,v);
                    nseeds += 1;
                    for (int b = 0; b < ninput_channels; b++) {
                        mean[b] += input->val[q][b];
                    }
                }
            }
        }
        iftDestroyMImage(&input);
    }

    for (int b = 0; b < ninput_channels; b++) {
        mean[b] = mean[b] / nseeds;
    }

    for (int i = 0; i < fs_seeds->n; i++) {
        basename = iftFilename(fs_seeds->files[i]->path, "-seeds.txt");
        if (strcmp(ext,"mimg") == 0){
            sprintf(filename, "%s/%s.mimg", inputdata_dir, basename);
            input = iftReadMImage(filename);
        } else {
            sprintf(filename, "%s/%s.%s", inputdata_dir, basename, ext);
            input = ReadInputMImage(filename);
        }
        iftFree(basename);

        ninput_channels = input->m;
        iftLabeledSet *S = iftMReadSeeds(input, fs_seeds->files[i]->path);
        while (S != nullptr) {
            int l;
            int p = iftRemoveLabeledSet(&S, &l);


            /*for (int b = 0; b < ninput_channels; b++) {
                stdev[b] += (input->val[p][b] - mean[b]) *
                            (input->val[p][b] - mean[b]);
            }*/

            iftVoxel u = iftMGetVoxelCoord(input,p);
            for (int a = 0; a < A->n; a++) {
                iftVoxel v = iftGetAdjacentVoxel(A,u,a);
                if (iftMValidVoxel(input,v)) {
                    int q = iftMGetVoxelIndex(input,v);
                    for (int b = 0; b < ninput_channels; b++) {
                        stdev[b] += (input->val[q][b] - mean[b]) *
                                    (input->val[q][b] - mean[b]);
                    }
                }
            }
        }
        iftDestroyMImage(&input);
    }

    for (int b = 0; b < ninput_channels; b++) {
        stdev[b] = sqrtf(stdev[b] / nseeds) + stdev_factor;
    }

}

iftMImage *ReadInputMImage(char *filename) {
    iftImage  *img     = iftReadImageByExt(filename);
    iftMImage *input   = iftImageToMImage(img, LABNorm2_CSPACE);
    iftDestroyImage(&img);
    return (input);
}

iftMatrix *SelectRelevantKernelsByPCA(iftMatrix *kernels, int number_of_kernels) {
    iftMatrix *kern_t = iftTransposeMatrix(kernels);
    iftDataSet *Z = iftFeatureMatrixToDataSet(kern_t);
    iftDestroyMatrix(&kern_t);

    iftSetStatus(Z, IFT_TRAIN);
    Z->fsp.mean = iftAllocFloatArray(Z->nfeats); /* ignoring centralization */

    iftMatrix *kern_pca = iftRotationMatrixByPCA(Z);
    iftMatrix *kern_rot = iftTransposeMatrix(kern_pca);
    iftDestroyMatrix(&kern_pca);

    number_of_kernels = iftMin(number_of_kernels, kern_rot->ncols);
    iftMatrix *M = iftCreateMatrix(number_of_kernels, kern_rot->nrows);

    for (int k = 0; k < number_of_kernels; k++) {
        for (int j = 0; j < kern_rot->nrows; j++) {
            iftMatrixElem(M, k, j) = iftMatrixElem(kern_rot, k, j);
        }
    }

    iftDestroyMatrix(&kern_rot);
    iftDestroyDataSet(&Z);

    return (M);
}

iftMatrix *SelectRelevantKernelsByKmeans(iftMatrix *K, int ngroups) {
    iftMatrix *Kt = iftTransposeMatrix(K);
    iftDataSet *Z = iftFeatureMatrixToDataSet(Kt);
    iftDestroyMatrix(&Kt);

       iftRandomSeed(42);


    iftMatrix *kernels = iftCreateMatrix(ngroups, Z->nfeats);

    iftKMeans(Z, ngroups, 100, float(0.001), nullptr, nullptr, iftCosineDistance);
    int i = 0;
    for (int s = 0; s < Z->nsamples; s++) {
        if (iftHasSampleStatus(Z->sample[s], IFT_PROTOTYPE)) {
            iftUnitNorm(Z->sample[s].feat, Z->nfeats);
            for (int j = 0; j < Z->nfeats; j++)
                iftMatrixElem(kernels, i, j) = Z->sample[s].feat[j];
            i++;
        }
    }
    iftDestroyDataSet(&Z);
    return (kernels);
}

iftMatrix *ComputeKernelBank(iftDataSet *Z, int *ngroups, uchar grouping_method) {
    iftMatrix *kernels = NULL;
    int i = 0;

    if (Z->nsamples < 1) {
        *ngroups = 0;
        return iftCreateMatrix(0,0);
    }

    iftRandomSeed(42);

    if (*ngroups >= Z->nsamples) { /* choose one cluster for this marker */
      *ngroups = 1;
    }


    switch (grouping_method) {
        case 0: /* KMeans */
          kernels = iftCreateMatrix(*ngroups, Z->nfeats);
          iftKMeans(Z, *ngroups, 100, 0.001, NULL, NULL, iftEuclideanDistance);
          for (int s = 0; s < Z->nsamples; s++) {
            if (iftHasSampleStatus(Z->sample[s], IFT_PROTOTYPE)) {
              iftUnitNorm(Z->sample[s].feat, Z->nfeats);
              for (int j = 0; j < Z->nfeats; j++) {
                iftMatrixElem(kernels, i, j) = Z->sample[s].feat[j];
              }
              i++;
            }
          }
        break;
    }

    return (kernels);
}


iftMatrix *LearnKernelBank(iftMImage *input, iftLabeledSet *M, int* kernel_labels,  iftAdjRel *A, int nsamples, int nkernels_per_marker) {
    int tensor_size     = input->m * A->n;
    iftDataSet *Z       = ComputeSeedDataSet(input, M, A, nsamples);
    iftMatrix **kernels = (iftMatrix **) calloc(Z->nclasses + 1, sizeof(iftMatrix *));
    int total_nkernels  = 0;

    for (int c = 1; c <= Z->nclasses; c++) {
        iftDataSet *Z1  = iftExtractSamplesFromClass(Z, c);
        int nkernels    = nkernels_per_marker;
    /* 0: kmeans, 1: iterated watershed and 2: OPF c clusters */
        kernels[c]      = ComputeKernelBank(Z1, &nkernels, 0);
        total_nkernels += nkernels;
        iftDestroyDataSet(&Z1);
    }

    // last row will be kernel's class
    iftMatrix *kernelbank = iftCreateMatrix(total_nkernels, tensor_size);

    int k = 0;
    for (int c = 1; c <= Z->nclasses; c++) {
        for (int col = 0; col < kernels[c]->ncols; col++, k++) {
            for (int row = 0; row < kernels[c]->nrows; row++) {
                iftMatrixElem(kernelbank, k, row) = iftMatrixElem(kernels[c], col, row);
            }
            kernel_labels[k] = c;
        }
    }

    for (int c = 0; c <= Z->nclasses; c++) {
        iftDestroyMatrix(&kernels[c]);
    }
    iftFree(kernels);
    iftDestroyDataSet(&Z);

//    if (kernelbank->ncols > nkernels_per_image) { /* force a number of kernels per image */
//      iftMatrix *Rkernels = SelectRelevantKernelsByKmeans(kernelbank, nkernels_per_image);
//      iftDestroyMatrix(&kernelbank);
//      kernelbank = Rkernels;
//    }

   return (kernelbank);
}


iftDataSet *ComputeSeedDataSet(iftMImage *img, iftLabeledSet *S, iftAdjRel *A, int nsamples) {
    int tensor_size = int(img->m) * A->n;
    iftDataSet *Z = iftCreateDataSet(nsamples, tensor_size);
    int ninput_channels = int(img->m);

    Z->nclasses = 0;
    int s       = 0;
    iftLabeledSet *seed = S;
    while (seed != nullptr) {
        int p = seed->elem;
        Z->sample[s].id = s;
        Z->sample[s].truelabel = seed->label;
        if (Z->sample[s].truelabel > Z->nclasses)
            Z->nclasses = Z->sample[s].truelabel;
        iftVoxel u = iftMGetVoxelCoord(img, p);
        int j = 0;
        for (int k = 0; k < A->n; k++) {
            iftVoxel v = iftGetAdjacentVoxel(A, u, k);
            if (iftMValidVoxel(img, v)) {
                int q = iftMGetVoxelIndex(img, v);
                for (int b = 0; b < ninput_channels; b++) {
                    Z->sample[s].feat[j] = img->val[q][b];
                    j++;
                }
            } else {
                for (ulong b = 0; b < img->m; b++) {
                    Z->sample[s].feat[j] = 0;
                    j++;
                }
            }
        }
        s++;
        seed = seed->next;
    }

    iftSetStatus(Z, IFT_TRAIN);
    iftAddStatus(Z, IFT_SUPERVISED);

    return (Z);
}

void NormalizeImageByZScore(iftMImage *img, float *mean, float *stdev) {

#pragma omp parallel for
    for (unsigned long p = 0; p < ulong(img->n); p++) {
        for (unsigned long b = 0; b < ulong(img->m); b++) {
            img->val[p][b] = (img->val[p][b] - mean[b]) / stdev[b];
        }
    }
}

void WriteMeanStdev(char *basepath, float *mean, float *stdev, long ninput_channels) {
    char filename[2][200];
    FILE *fp[2];

    sprintf(filename[0], "%s-mean.txt", basepath);
    sprintf(filename[1], "%s-stdev.txt", basepath);
    fp[0] = fopen(filename[0], "w");
    fp[1] = fopen(filename[1], "w");
    for (int b = 0; b < ninput_channels; b++) {
        fprintf(fp[0], "%f ", double(mean[b]));
        fprintf(fp[1], "%f ", double(stdev[b]));
    }
    fclose(fp[0]);
    fclose(fp[1]);
}
