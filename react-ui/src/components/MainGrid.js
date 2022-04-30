import React, { useState } from 'react';
const mqtt = require("mqtt");

const client = mqtt.connect("ws://3.108.52.230:9001");

client.on("connect", function () {
    client.subscribe("mq2/calc_data");
});

const MainGrid = () => {

    const [data, setdata] = useState(null)

    client.on("message", function (topic, message) {
        setdata(JSON.parse(message.toString()));
    });

    return (
        <div className='main-grid-container'>
            <div className='temperature'>
                <div className="data">
                    <div className="value">
                        {
                            data === null ?
                                0 :
                                data["temp"]["value"].toFixed(2)
                        }
                    </div>
                    <div className="unit">
                        {
                            data === null ?
                            0 :
                            data["temp"]["unit"]==="celsius"?
                            " \u00b0C":
                            "&deg;"
                        }
                    </div>
                </div>
            </div>
            <div className='raw-data'>
                <div className="data">
                    <div className="value">
                        {
                            data === null ?
                                0 :
                                data["raw_analog"]["value"]
                        }
                    </div>
                    <div className="unit">
                        {
                            data === null ?
                            "" :
                            data["raw_analog"]["unit"]
                        }
                    </div>
                </div>
            </div>
            <div className='co-cls'>
                <div className="data">
                    <div className="value">
                        {
                            data === null ?
                                0 :
                                data["co"]["value"]
                        }
                    </div>
                    <div className="unit">
                        {
                            data === null ?
                            "":
                            data["co"]["unit"]
                        }
                    </div>
                </div>
            </div>
            <div className='gas'>
                <div className="data">
                    <div className="value">
                        {
                            data === null ?
                                0 :
                                data["lpg"]["value"]
                        }
                    </div>
                    <div className="unit">
                        {
                            data === null ?
                            "" :
                            data["lpg"]["unit"]
                        }
                    </div>
                </div>
            </div>
            <div className='smoke'>
                <div className="data">
                    <div className="value">
                        {
                            data === null ?
                                0 :
                                data["smoke"]["value"]
                        }
                    </div>
                    <div className="unit">
                        {
                            data === null ?
                            "" :
                            data["smoke"]["unit"]
                        }
                    </div>
                </div>
            </div>
        </div>
    )
}

export default MainGrid