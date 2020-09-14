/*
 * Copyright (c) 2020
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include "libavfilter/dnn/dnn_backend_native_layer_avgpool.h"

#define EPSON 0.00001

static int test_with_same(void)
{
    // the input data and expected data are generated with below python code.
    /*
    import tensorflow as tf
    import numpy as np

    x = tf.placeholder(tf.float32, shape=[1, None, None, 3])
    y = tf.layers.average_pooling2d(x, pool_size=[2,2], strides=[1,1], padding='VALID')
    data = np.random.rand(1, 5, 6, 3);

    sess=tf.Session()
    sess.run(tf.global_variables_initializer())

    output = sess.run(y, feed_dict={x: data})

    print("input:")
    print(data.shape)
    print(list(data.flatten()))

    print("output:")
    print(output.shape)
    print(list(output.flatten()))
    */

    AvgPoolParams params;
    DnnOperand operands[2];
    int32_t input_indexes[1];
    float input[1*5*6*3] = {
        0.7461309859908424, 0.7567538372797069, 0.07662743569678687, 0.8882112610336333, 0.9720443314026668, 0.3337200343220823, 0.4421032129780248,
        0.14940809044964876, 0.6773177061961277, 0.9778844630669781, 0.6522650522626998, 0.0317651530878591, 0.31259897552911364, 0.6235936821891896,
        0.40016094349542775, 0.4599222930032276, 0.7893807222960093, 0.8475986363538283, 0.5058802717647394, 0.7827005363222633, 0.3032188123727916,
        0.8983728631302361, 0.20622408444965523, 0.22966072303869878, 0.09535751273161308, 0.8760709100995375, 0.9982324154558745, 0.7904595468621013,
        0.13883671508879347, 0.9332751439533138, 0.0010861680752152214, 0.3607210449251048, 0.6600652759586171, 0.7629572058138805, 0.29441975810476106,
        0.2683471432889405, 0.22574580829831536, 0.8893251976212904, 0.3907737043801005, 0.6421829842863968, 0.6670373870457297, 0.9383850793160277,
        0.4120458907436003, 0.3589847212711481, 0.48047736550128983, 0.6428192648418949, 0.0313661686292348, 0.429357100401472, 0.5123413386514056,
        0.8492446404097114, 0.9045286128486804, 0.8123708563814285, 0.3943245008451698, 0.9576713003177785, 0.5985610965938726, 0.9350833279543561,
        0.8010079897491659, 0.45882114217642866, 0.35275037908941487, 0.4555844661432271, 0.12352455940255314, 0.37801756635035544, 0.2824056214573083,
        0.6229462823245029, 0.7235305681391472, 0.5408259266122064, 0.12142224381781208, 0.34431198802873686, 0.7112823816321276, 0.6307144385115417,
        0.8136734589018082, 0.842095618140585, 0.8602767724004784, 0.6649236853766185, 0.5184782829419623, 0.9119607270982825, 0.3084111974561645,
        0.39460705638161364, 0.17710447526170836, 0.1715485945814199, 0.17277563576521882, 0.40188232428735704, 0.22847985411491878, 0.4135361701550696,
        0.24621846601980057, 0.6576588108454774, 0.6063336087333997, 0.6452342242996931, 0.7071689702737508, 0.1973416063225648
    };
    float expected_output[] = {
        0.75964886, 0.6794307, 0.23580676, 0.5810112, 0.5509369, 0.55973274, 0.5764512, 0.45414522, 0.6601476, 0.52050734, 0.44385415,
        0.50631666, 0.38414115, 0.5170288, 0.544043, 0.61143976, 0.5419003, 0.5579729, 0.5680455, 0.6363218, 0.4655096, 0.51198983,
        0.5270792, 0.66168886, 0.48517057, 0.3513146, 0.7103355, 0.48667657, 0.34504217, 0.7318065, 0.5221889, 0.4746775, 0.69765306,
        0.78766406, 0.34437215, 0.6130092, 0.48132777, 0.7110491, 0.6464378, 0.40914366, 0.4391975, 0.5392131, 0.45033398, 0.37297475,
        0.43326652, 0.4748823, 0.48711336, 0.64649844, 0.51921225, 0.60038865, 0.8538945, 0.7215426, 0.60399896, 0.89988345, 0.707405,
        0.5652921, 0.54241943, 0.41785273, 0.30268195, 0.3263432, 0.3313644, 0.37539417, 0.35238582, 0.34811732, 0.48849532, 0.56799453,
        0.41089734, 0.63070333, 0.5892633, 0.6379743, 0.7604212, 0.5197186, 0.88611877, 0.48666745, 0.45654267, 0.5445326, 0.2399799,
        0.28369135, 0.28949338, 0.20001422, 0.2931559, 0.3240504, 0.44306934, 0.5099349, 0.44572634, 0.68241394, 0.40183762, 0.6452342,
        0.707169, 0.1973416
    };
    float *output;

    params.strides = 1;
    params.kernel_size = 2;
    params.padding_method = SAME;

    operands[0].data = input;
    operands[0].dims[0] = 1;
    operands[0].dims[1] = 5;
    operands[0].dims[2] = 6;
    operands[0].dims[3] = 3;
    operands[1].data = NULL;

    input_indexes[0] = 0;
    dnn_execute_layer_avg_pool(operands, input_indexes, 1, &params, NULL);

    output = operands[1].data;
    for (int i = 0; i < sizeof(expected_output) / sizeof(float); ++i) {
        if (fabs(output[i] - expected_output[i]) > EPSON) {
            printf("at index %d, output: %f, expected_output: %f\n", i, output[i], expected_output[i]);
            av_freep(&output);
            return 1;
        }
    }

    av_freep(&output);
    return 0;
}

static int test_with_valid(void)
{
    // the input data and expected data are generated with below python code.
    /*
    import tensorflow as tf
    import numpy as np

    x = tf.placeholder(tf.float32, shape=[1, None, None, 3])
    y = tf.layers.average_pooling2d(x, pool_size=[2,2], strides=[1,1], padding='VALID')
    data = np.random.rand(1, 5, 6, 3);

    sess=tf.Session()
    sess.run(tf.global_variables_initializer())

    output = sess.run(y, feed_dict={x: data})

    print("input:")
    print(data.shape)
    print(list(data.flatten()))

    print("output:")
    print(output.shape)
    print(list(output.flatten()))
    */

    AvgPoolParams params;
    DnnOperand operands[2];
    int32_t input_indexes[1];
    float input[1*5*6*3] = {
        0.5046741692941682, 0.9273653202485155, 0.8193878359859937, 0.1904059431360905, 0.8664919633253656, 0.7484625128286059, 0.984534184632278,
        0.31900804890072254, 0.3259426099940872, 0.05388974903570376, 0.7356610151331133, 0.46710858713311965, 0.718553768817036, 0.062478421853278676,
        0.7813224786584609, 0.4826837517658389, 0.9748095400220147, 0.8078547703898341, 0.11976750668368585, 0.8713586777195065, 0.41447321551284355,
        0.9818788239089807, 0.4335715767584073, 0.4059793452147419, 0.3677205907204525, 0.47919995923571, 0.8341395256258882, 0.7059726374074609,
        0.5478504551919791, 0.8622900484790175, 0.8343709722511167, 0.05089827275068537, 0.6465283980840416, 0.544539116066677, 0.39812057257884337,
        0.9578115576866337, 0.25012888117580145, 0.579333516024662, 0.5556732133051457, 0.6119862111181243, 0.0018736758772316398, 0.9795490254040474,
        0.4488085008883018, 0.28947489777011737, 0.4834108668633247, 0.9280490084385024, 0.9895821458049648, 0.31777618554697606, 0.42679693258977847,
        0.74447844466923, 0.9752225305081498, 0.17564130841849335, 0.22382692067314292, 0.009602884447469373, 0.5144884415025782, 0.031622570708844555,
        0.8277532752502512, 0.4111593210409763, 0.5272084646575664, 0.28856508082905297, 0.11317726946036655, 0.7203328275540273, 0.8310055019972384,
        0.8535951508685228, 0.40230347305233227, 0.2819703265132867, 0.6243143957791139, 0.7512463693822311, 0.7523056340495644, 0.8838077258040928,
        0.5472240664033092, 0.2550538284454935, 0.5560317774456567, 0.8966847087518931, 0.6728358284165321, 0.30361297147530875, 0.464343925441822,
        0.34507695659461224, 0.6333175615390685, 0.26661369038523497, 0.9926748632253231, 0.9994267301382666, 0.8684917986974414, 0.3598754806113009,
        0.49550268625464666, 0.03652458679973214, 0.13469081713137177, 0.4579424049273835, 0.48641107969110353, 0.9670250266945365
    };
    float expected_output[1*4*5*3] = {
        0.44918162, 0.7746969, 0.5970757, 0.63113487, 0.5245679, 0.578631, 0.52802926, 0.52042985, 0.6223702, 0.57819676, 0.34922206,
        0.6893124, 0.64503694, 0.37157673, 0.7983793, 0.49094033, 0.47153437, 0.5889187, 0.6025985, 0.30103004, 0.6757697, 0.6126377,
        0.5765268, 0.62440413, 0.7237974, 0.5832023, 0.7004543, 0.49533707, 0.35433105, 0.6472913, 0.44694072, 0.28500956, 0.6628852,
        0.39628282, 0.38472247, 0.6456326, 0.58590746, 0.60042334, 0.47854072, 0.7081889, 0.7219026, 0.5818187, 0.5276401, 0.56669396,
        0.49804622, 0.4463231, 0.4799649, 0.5335578, 0.36531678, 0.4946247, 0.6143306, 0.6498792, 0.5644355, 0.6163815, 0.7432098,
        0.5146416, 0.38221055, 0.6153918, 0.45535153, 0.5272688
    };
    float *output;

    params.strides = 1;
    params.kernel_size = 2;
    params.padding_method = VALID;

    operands[0].data = input;
    operands[0].dims[0] = 1;
    operands[0].dims[1] = 5;
    operands[0].dims[2] = 6;
    operands[0].dims[3] = 3;
    operands[1].data = NULL;

    input_indexes[0] = 0;
    dnn_execute_layer_avg_pool(operands, input_indexes, 1, &params, NULL);

    output = operands[1].data;
    for (int i = 0; i < sizeof(expected_output) / sizeof(float); ++i) {
        if (fabs(output[i] - expected_output[i]) > EPSON) {
            printf("at index %d, output: %f, expected_output: %f\n", i, output[i], expected_output[i]);
            av_freep(&output);
            return 1;
        }
    }

    av_freep(&output);
    return 0;
}

int main(int argc, char **argv)
{
    if (test_with_same())
        return 1;
    if (test_with_valid())
        return 1;

    return 0;
}