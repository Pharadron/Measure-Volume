const controller = {};
const {validationResult} = require('express-validator');


const {
    InfluxDB,
    Point
} = require("@influxdata/influxdb-client");
const token = "B4Svc-_51XYr8JR9ZxK3F0U5AnuJEkBlac57Y1CjTee4SFkkfk3A8-Vwz7--TJLexfdvaSq7pIHPY42mbSqi0w=="
const url = "https://us-east-1-1.aws.cloud2.influxdata.com"
const client = new InfluxDB({
    url,
    token
})
let org = "1d9573dc3ebd632b"
let bucket = "IOS"

let writeClient = client.getWriteApi(org, bucket, 'ns');

// let point = new Point('dashboard37')
// // .tag('device','abc')
// .intField('Sonic', 99);
// void setTimeout(() => {
// writeClient.writePoint(point)
// }, 1000) // separate points by 1 second
// void setTimeout(() => {
// writeClient.flush()
// }, 5000)

controller.Dist = async (req, res) => {
    let Avalue = null;
    let Bvalue = null;
    let Cvalue = null;
    let Dvalue = null;
    let SUMvalue = null;
    let SUM2value = null;
    let VALUET = null;
    let VALUET1 = null;
    let PRIMARTvalue = null;
    let STA1 = null;
    let STA2 = null;
    let STA3 = null;
    let A1 = null;
    let A2 = null;
    let B2 = null;
    let C3 = null;
    let D4 = null; // เพิ่มการประกาศ Rvalue ที่นี่

    try {
        const queryClient = client.getQueryApi(org);

        const fluxQuery = `from(bucket: "IOS")
          |> range(start: -15m)
          |> filter(fn: (r) => r["device"] == "cc:50:e3:c:32:58")
          |> filter(fn: (r) => r._measurement == "PM" and (r["_field"] == "A" or r["_field"] == "B" or r["_field"] == "C" or r["_field"] == "D" or r["_field"] == "SUM" or r["_field"] == "SUM2" or r["_field"] == "VALUE" or r["_field"] == "PRIMART")) // เพิ่มการกรองเพื่อเลือกค่า Sonic และ R
          |> last()`;

        await new Promise((resolve, reject) => {
            queryClient.queryRows(fluxQuery, {
                next: (row, tableMeta) => {
                    const tableObject = tableMeta.toObject(row);
                    console.log(tableObject._value);
                    if (tableObject._field === "A") {
                        Avalue = tableObject._value;
                    } else if (tableObject._field === "B") {
                        Bvalue = tableObject._value;
                    } else if (tableObject._field === "C") {
                        Cvalue = tableObject._value;
                    } else if (tableObject._field === "D") {
                        Dvalue = tableObject._value;
                    } else if (tableObject._field === "SUM") {
                        SUMvalue = tableObject._value;
                    } else if (tableObject._field === "SUM2") {
                        SUM2value = tableObject._value;
                    } else if (tableObject._field === "VALUE") {
                        VALUET = tableObject._value;
                    } else if (tableObject._field === "PRIMART") {
                        PRIMARTvalue = tableObject._value;
                    }
                },
                error: (error) => {
                    console.error("\nError", error);
                    reject(error);
                },
                complete: () => {
                    resolve();
                },
            });
        });


        if (VALUET == 1) { //ลูกบาศ
            A1 = "(กว้าง)";
            A2 = "ลบ.ซม.";
            VALUET1 ="ปริมาตรทั้งหมด";
            B2 = "(ยาว)";
            C3 = "(สูง)";
            D4 = "(ระยะห่างเซนเซอร์)";
            STA1 = "กำลังทำงาน";
            STA2 = "ไม่ได้ใช้งาน";
            STA3 = "ไม่ได้ใช้งาน";
        } else if (VALUET == 2) { //กระบอก
            A1 = "(รัศมี)";
            A2 = "ลบ.ซม.";
            B2 = "(สูง)";
            VALUET1 ="ปริมาตรทั้งหมด";
            C3 = "(ระยะห่างเซนเซอร์)";
            D4 = "(ไม่มี)";
            STA1 = "ไม่ได้ใช้งาน";
            STA2 = "กำลังทำงาน";
            STA3 = "ไม่ได้ใช้งาน";
        } else if (VALUET == 3) { //ระดับ
            A1 = "(ความลึก)";
            A2 = "ซม." ;
            VALUET1 ="ระยะขอบถึงก้นบ่อ";
            B2 = "(ระยะห่างเซนเซอร์)";
            C3 = "(ไม่มี)";
            D4 = "(ไม่มี)";
            STA1 = "ไม่ได้ใช้งาน";
            STA2 = "ไม่ได้ใช้งาน";
            STA3 = "กำลังทำงาน";
        }
        res.render("DistView", {
            data1: {
                Avalue,
                Bvalue,
                Cvalue,
                Dvalue,
                SUMvalue,
                SUM2value,
                VALUET,
                VALUET1,
                PRIMARTvalue,
                STA1,
                STA2,
                STA3,
                A1,
                A2,
                B2,
                C3,
                D4
            }
        }); // ส่ง Sonicvalue และ Rvalue ไปยังหน้าเว็บ
    } catch (error) {
        console.error("Error", error);
        res.render("DistView", {
            data1: {
                Avalue,
                Bvalue,
                Cvalue,
                Dvalue,
                SUMvalue,
                SUM2value,
                VALUET,
                VALUET1,
                PRIMARTvalue,
                STA1,
                STA2,
                STA3,
                A1,
                A2,
                B2,
                C3,
                D4
            }
        }); // ส่ง Sonicvalue และ Rvalue ไปยังหน้าเว็บ
    }
};



module.exports = controller;