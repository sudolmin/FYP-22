import * as React from 'react';
import Box from '@mui/material/Box';
import Drawer from '@mui/material/Drawer';
import { Button, IconButton, TextField } from '@mui/material';
import SettingsIcon from '@mui/icons-material/Settings';
import { encryptdata, decryptdata } from '../utils/encryptionHandler';
const mqtt = require("mqtt");

const configTopic = "mq2/config"

const client = mqtt.connect("ws://3.108.52.230:9001");
client.on("connect", function () {
    client.subscribe(configTopic);
});

client.on("message", function (topic, message) {
    localStorage.setItem("encryptedConfig", message.toString());
});

export default function TDrawer() {
    const [state, setState] = React.useState({
        top: false,
        left: false,
        bottom: false,
        right: false,
    });

    const [config, setConfig] = React.useState({})

    React.useMemo(() =>{
            console.log("Running memo func");
            if (localStorage.getItem("encryptedConfig")!==null) {
                var decconfig = decryptdata(localStorage.getItem("encryptedConfig"));
                setConfig(JSON.parse(decconfig));
            }
    }, [localStorage.getItem("encryptedConfig")]);

    const toggleDrawer = (anchor, open) => (event) => {
        if (event.type === 'keydown' && (event.key === 'Tab' || event.key === 'Shift')) {
            return;
        }

        setState({ ...state, [anchor]: open });
    };

    function saveConfig() {
        var encConfig = encryptdata(config);
        // publish and retaining the data
        client.publish(configTopic, encConfig, {retain: true})
    }

    const list = (anchor) => (
        <Box
            sx={{ width: 'auto' }}
            role="presentation"
            className='top-drawer'
        // onClick={toggleDrawer(anchor, false)}
        // onKeyDown={toggleDrawer(anchor, false)}
        >
            <div className="form-container">
                <TextField 
                onChange={(e)=>{
                    var newData = {...config, "phn_no": e.target.value}
                    setConfig(newData);
                }}
                value={config["phn_no"]===undefined?"":config["phn_no"]} 
                fullWidth id="phone-number" label="Phone Number" variant="filled" />
                <TextField 
                onChange={(e)=>{
                    var newData = {...config, "email_id": e.target.value}
                    setConfig(newData);
                }}
                value={config["email_id"]===undefined?"":config["email_id"]} 
                fullWidth 
                id="email-addr" label="Email Address(s)" variant="filled"
                helperText="Enter the email addresses separated by `;` if more than one." />
                <TextField 
                onChange={(e)=>{
                    var newData = {...config, "raw_limit": e.target.value}
                    setConfig(newData);
                }}
                value={config["raw_limit"]===undefined?"":config["raw_limit"]} 
                fullWidth
                inputProps={{ inputMode: 'numeric', pattern: '[0-9]*' }}  
                id="raw-limit" label="Raw Limit" variant="filled" 
                helperText="Notification via email and sms would be delivered when limit entered is exceeded." />

                <Button 
                variant='contained'
                onClick={saveConfig}
                >Save Config</Button>
            </div>
        </Box>
    );

    return (
        <div className='open-drawer-btn'>
            {['top'].map((anchor) => (
                <React.Fragment key={anchor}>
                    <IconButton onClick={toggleDrawer(anchor, true)}>
                        <SettingsIcon style={{color: '#22e895', fontSize: "3rem"}} />
                    </IconButton>
                    <Drawer
                        anchor={anchor}
                        open={state[anchor]}
                        onClose={toggleDrawer(anchor, false)}
                    >
                        {list(anchor)}
                    </Drawer>
                </React.Fragment>
            ))}
        </div>
    );
}
