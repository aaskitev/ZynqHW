# Лабораторная работа 7

## Добавление кастомное IP в каталог Vivado IP

## Обзор

Xilinx предоставляет широкий каталог IP-ядер, которые позволяют осуществить очень легкое подключение большого количества интерфейсов в аш дизайн. Однако наибольшая ценность Zynq заключается в сочетании кастомного IP-ядра с двухъядерной процессорной системой ARM. В этой лабораторной работе представлена пошаговая инструкция как создать кастомное IP, добавить его в IP-каталог и устанавливать соединение с нашим дизайном.

## Цель работы

После выполнения лабораторной работы вы сможете:

* Создавать новый IP проект
* Кастомизировать IP
* Добавлять его в IP-каталог Vivado
* Добавлять кастомное IP в ваш проект
* Добавлять прерывание в процессорную систему и соединять с кастомным IP
* Тестировать кастомный IP кастомным приложением

## Эксперимент 1: Создание нового IP проекта

Этот эксперимент покажет, создать IP-проект, определить AXI-интерфейс и зарегистрировать настройки для IP.

---

### **Обобщенная инструкция:**

Создайте новую периферию AXI.

---

### **Пошаговая инструкция:**

1. Для данной лабораторной работы необходимо использовать существующий проект из шестой лабораторной работы.

2. Откройте существующий проект в *Vivado*.

3. Выберите **Tools->Create and Pacakege New IP...**

    ![Создать новый IP](./resources/lab7/Create%20New%20IP.png)

4. Нажмите кнопку **Next** в приветственном окне.

5. Мы будем создавать новую AXI-периферию, поэтому отметьте пункт **Create a New AXI4 peripheral**. Нажмите кнопку **Next**.
    ![Выбор создания новой AXI периферии](./resources/lab7/Create%20a%20New%20AXI%20Peripheral.png)

6. Введите в соответствующие поля следующую информацию:
    * Name: `PWM_w_Int`
    * Version: `1.0`
    * Display name: `PWM_w_Int_v1.0`
    * Description: `PWM with Interrupt option`
    * IP location: `U:/ZynqLabs/ip_repo`

    Нажмите кнопку **Next**.

    ![Информация об IP](./resources/lab7/IP%20Details.png)

7. Создаваемая периферия достаточно проста и не требует большой пропускной способности шины, поэтому достаточно подключить интерфейс AXI-Lite. Устройство также будет ведомым (slave) относительно процессора (master). Для наших целей нам достаточно иметь 32 битный интерфейс и один регистр. Однако четыре - минимальное количество регистров. Таким образом, нам достаточно параметров **по-умолчанию**. Нажмите кнопку **Next**.

    ![Настройки интерфейса IP](./resources/lab7/IP%20Interface%20Settings.png)

8. На последнем шаге выберите **Add IP to the repository**. Проверьте путь до каталога. Нажмите **Finish**.

    ![Итоги создания IP](./resources/lab7/IP%20Creation%20Summary.png)

### **Вопросы:**

* Где расположен проект IP-ядра?

## Эксперимент 2: Кастомизация нового IP-проекта

Этот эксперимент покажет, как добавить пользовательскую логику в IP-ядро. Также мы изменим наше IP-ядро, добавив в него новый порт и параметры.

![Схема Дизайна](./resources/lab7/Design%20Scheme.png)

---

### **Обобщенная инструкция:**

Импортируйте пользовательский код описания аппаратуры **PWM_Controller_Int.v** и подключите его в проект. Соедините с **slv_reg0** из интерфейса AXI. Соедините выходы *PWM* с модулем верхнего уровня.

---

### **Пошаговая инструкция:**

1. В *Vivado* выберите **Window->IP Catalog**.

2. Выберите IP в каталоге IP, введя в поиске `PWM`. Нажмите правой кнопкой мыши и выберите **Edit in IP Packager**.

    ![Отредактировать IP](./resources/lab7/Edit%20IP%20Project.png)

3. Мы принимаем настройки имени и расположения проекта по-умолчанию и нажимаем **OK**.

    ![Имя и расположения проекта IP](./resources/lab7/IP%20Project%20Name%20and%20Location.png)

    По завершению откроется новое окно *Vivado Project*. В этом новом окне мы будем редактировать наше IP-ядро. Обратите внимание на корневой каталог (Root Directory) нашего проекта и его имя (Project Name). Это будет важно при дальнейшем выполнении работы. \<IP Name\>_project.xpr - проект, который будет открыт для редактирования IP. В нашем случае наш проект IP назван `PWM_w_Int_v1_0_project.xpr`.

4. В окне *Sources* раскройте пункт **Design Sources**. Обратите внимание на то, что, когда мы в окне с исходниками выбираем файл, его свойства открываются в окне *Source File Properties*. Это полезно для определения того, где расположены файлы с HDL на вашем компьютере.

    ![Исходники и их свойства](./resources/lab7/Design%20Source%20Properties.png)


5. В проекте присутствуют два Verilog файла. Первый `PWM_w_Int.v` - модуль-обертка верхнего уровня. Откройте этот файл дважды **нажав** на него. В районе 47 строчки он инстанцирует интерфейс AXI `PWM_w_Int_slave_lite_v1_0_S00_AXI`, который определен в файле `PWM_w_Int_slave_lite_v1_0_S00_AXI.v`. Далее находится место для добавления пользовательской логики, куда мы и добавим логику нашего IP.

    ![Расположение пользовательской логики](./resources/lab7/User%20Logic%20Location.png)

6. Теперь скопируйте **PWM_Controller.v** из папки `/Support_documents` в `U:\ZynqHW\ip_repo\PWM_w_Int_1_0\hdl`

7. В области *Flow Navigator* выберите **Add Sources**. Далее выберите **Create Design Sources**. После завершения нажмите кнопку **Next**.

    ![Добавление исходника](./resources/lab7/Add%20Design%20Sources.png)

8. В следующем окне выберите **Add Files**.

9. Измените директорию на `U:\ZynqHW\ip_repo\PWM_w_Int_1_0\hdl`. Нажмите дважды на **PWM_Controller_Int.v**, чтобы добавить его в проект.

10. Проверьте файл. Также удостоверьтесь, что *Copy Sources into IP Directory* не отмечено галочкой. Нажмите **Finish**.

    ![Добавление файла в проект](./resources/lab7/Add%20Files%20to%20Project.png)

11. Теперь файл появится в нашем окне с исходниками. Но не внутри нашего файла верхнего уровня. Это ожидаемо, поскольку мы не подключили этот код в файл верхнего уровня.

    ![Новый исходник в проекте](./resources/lab7/New%20Source%20in%20Project.png)

12. Откройте **PWM_Controller_Int.v**. Изучите файл и инстанцируйте этот модуль в  `PWM_w_Int.v`. Дважды нажмите на  `PWM_w_Int.v`.

13. Внизу файла `PWM_w_Int.v` Добавьте следующий код в секцию пользовательской логики в районе 74 строчки между комментариями *Add user logic here* и *User logic ends*:

    ```verilog
	// Add user logic here
    PWM_Controller_Int #(
        .period     ()
    ) PWM_Controller_Int_inst (
        .Clk        (),
        .DutyCycle  (), 
        .Reset      (),
        .PWM_out    (),
        .Interrupt  (),
        .count      ()
    );
	// User logic ends
    ```

    Нажмите сохранить. Это нормально, что текущий файл может показывать синтаксические ошибки - входы и выходы мы соединим позже.

14. Как только мы ввели этот код, наш файл с кодом логики IP будет внутри файла верхнего уровня

    ![Пользовательская логика тепеь в иерархии проекта](./resources/lab7/User%20Logic%20now%20in%20Project%20Hierarchy.png)

    Теперь необходимо подключить входы добавленной логики к существующей части IP-ядра.

15. Сейчас нам нужно соединить входы и выходы с нашей пользовательской логики. Начните с очевидных сигналов модуля пользовательской логики `PWM_Controller_Int`. Тактовые сигналы и сигналы сброса имеют прямую ассоциацию между сигналами контроллера и Slave AXI.
    
    ```verilog
    ...
    .Clk        (s00_axi_aclk),
    ...
    .Reset      (s00_axi_aresetn),
    ...
    ```

    Теперь соединим выходы. Эти сигналы должны быть объявлены в списке портов модуля верхнего уровня. У этого IP будет 4 выхода.
        
    * **LEDs**: выходы *PWM*, соединенные с LED'ами.
    * **Interrupt_out**: сигнал прерывания, показывающий о некорректных настройках *PWM*.
    * **PWM_Counter**: (для отладки) работающий в свободном режиме счетчик *PWM*.
    * **DutyCycle**: (Для отладки) Значение переданное с *PS* через интерфейс *AXI* для контролирования скважности *PWM*.
    * Добавим также настраиваемый параметр **PWM_PERIOD**, который устанавливает глубину счетчика *PWM*.

17. Добавим следующие порты в самом верху файла с кодом (около 5 строчки) и пользовательскую HDL логику:

    ```verilog
	module PWM_w_Int #
	(
		// Users to add parameters here
        parameter integer PWM_PERIOD = 20,
		// User parameters ends
		// Do not modify the parameters beyond this line

		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 4
	)
	(
		// Users to add ports here
        output wire [7:0]   LEDs,
        output wire         Interrupt_out,
        output wire [PWM_PERIOD-1:0] PWM_Counter,
        output wire [31:0]  DutyCycle,
		// User ports ends
        ...
    ```

    Полное подключение `PWM_Controller_Int` в районе 78-ой строчки:

    ```verilog
	// Add user logic here
    PWM_Controller_Int #(
        .period     (PWM_PERIOD)
    ) PWM_Controller_Int_inst (
        .Clk        (s00_axi_aclk),
        .DutyCycle  (DutyCycle), 
        .Reset      (s00_axi_aresetn),
        .PWM_out    (LEDs),
        .Interrupt  (Interrupt_out),
        .count      (PWM_Counter)
    );
	// User logic ends
    ```

    Таким образом, мы полностью подсоединили наш IP. Однако мы еще не соединили источник *DutyCycle*. Этот сигнал идет от процессора. К нашему удобству, для этих целей во время запуска *Create IP Peripheral wizard* Vivado создал для нас *AXI proxy*.

18. Откройте **PWM_w_Int_slave_lite_v1_0_S00_AXI.v**. Просмотрите файл на строчках 105 и ниже. Здесь вы увидите объяснения для четырех регистров, созданных *Vivado*. Далее идет логика, реализующая операции записи и чтения в эти регистры. Нам нужен только один из них - **slv_reg0**.

19. Сделаем *slv_reg0* выходом нашего модуля. В районе 18-ой строчки добавьте выход (**slave_reg0**, к примеру) в области для пользовательских портов. Затем соедините его с slv_reg0 (в районе 304 строчки):

    ```verilog
		// Users to add ports here
        output wire [C_S_AXI_ADDR_WIDTH-1 : 0]  slave_reg0,
		// User ports ends
        ...
    ```

    ```verilog
        ...
        // Add user logic here
        assign slave_reg0 = slv_reg0;
        // User logic ends
    ```

20. **Сохраните** файл, нажав на **Ctrl-S**, и вернитесь обратно к файлу модуля верхнего уровня иерархии `PWM_w_Int.v`.

    Так как мы добавили новый порт к нашему слейву AXI, мы должны соединить его с модулем верхнего уровня. Напомним, что этот регистр содержит значение **DutyCycle**, переданное от процессора.

21. Добавьте порт *slave_reg0* к модулю слейв AXI-интерфейса (в районе 55 строчки) и подключите новый выход к сигналу DutyCycle:

    ```verilog
    // Instantiation of Axi Bus Interface S00_AXI
        PWM_w_Int_slave_lite_v1_0_S00_AXI # ( 
            .C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
            .C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
        ) PWM_w_Int_slave_lite_v1_0_S00_AXI_inst (
            .slave_reg0 (DutyCycle), // Подключили выход slave_reg0 к сигналу DutyCycle
            .S_AXI_ACLK(s00_axi_aclk),
            .S_AXI_ARESETN(s00_axi_aresetn),
            .S_AXI_AWADDR(s00_axi_awaddr),
            .S_AXI_AWPROT(s00_axi_awprot),
            .S_AXI_AWVALID(s00_axi_awvalid),
        ...
    ```
    Сохраните файл.

22. На этом мы закончили проводить все соединения. Выберите файл модуля верхнего уровня `PWM_w_Int.v` и нажмите **Run Synthesis**, для проверки дизайна.

23. Не запускайте имплементацию. Откройте синтезированный дизайн из диалогового окна *Synthesis Completed*.

    ![Синтез окончен](./resources/lab7/Synthesis%20Completed.png)

24. Как только откроется синтезированный дизайн, откройте его схему, щелкнув по **Schematic** в разделе *Synthesis* в *Flow Navigator*. Проверьте, присутствуют ли все входы-выходы. Если схема выглядит правильно, то все должно быть хорошо. При реальном проектировании потребовалась бы дальнейшая проверка, но на данный момент мы можем упаковать наш IP.

## Эксперимент 3: Упаковка IP-проекта

Этот эксперимент покажет, как упаковать IP для IP-каталога.

---

### **Обобщенная инструкция:**

Упакуйте IP.

---

### **Пошаговая инструкция:**

1. Нажмите **Package IP** в окне *Flow Navigator*. Если вы редактируете уже упакованный IP, то будет кнопка **Edit Packaged IP**

    ![Упаковка IP](./resources/lab7/Package%20IP.png)

2. Откроется панель, которая поможет нам пошагово сбилдить IP для IP-каталога. В первом окне *IP Identification* имеется много параметров, которые мы вводили при создании проекта. **Проверьте эти параметры**. Обратите внимание на версию IP в этом окне. Если в проекте будут совершены изменения, значение версии может быть изменено здесь.

    ![Идентификация IP](./resources/lab7/IP%20Identification.png)

3. В следующем окне *Compatibility* мы можем выбрать семейства устройств, совместимых с нашим IP. *Zynq* включен сюда по-умолчанию, как целевое устройство, которое использовалось при создании IP. Больше устройств может быть добавлено, нажав на плюсик.

    ![Совместимость IP](./resources/lab7/IP%20Compatibility.png)

4. Следующее окно - *File Groups*. Обратите внимание на уведомление вверху окна. Это говорит о том, что *Vivado* заметил изменения, которые мы сделали в HDL. Нажмите на **Merge changes from File Groups Wizard**.

    ![Файлы IP](./resources/lab7/IP%20File%20Groups.png)

5. После того как наши изменения были объединены, раскройте все списки файлов. Обратите внимание, что средство слияния файлов добавило наш пользовательский HDL `PWM_Controller_Int`.

    ![Группы файлов](./resources/lab7/File%20Groups.png)

6. Так же у нас появилось предупреждение, что отсутствует документация на IP ядро. Давайте ее добавим. Для этого нажмите на кнопку со знаком плюс сверху и в открывшимся окне выберете **Product Guide**

    ![Добавим группы файлов](./resources/lab7/Add%20File%20Group.png)

7. Скопируйте файл `PWM_IP_Core_Datasheet.pdf` из  из папки `/Support_documents` в `U:\ZynqHW\ip_repo\PWM_w_Int_1_0\doc`. Теперь кликните правой кнопкой мыши по **Product Guide** и выберите **Add Files**.

8. Когда откроется окно добавления файлов, нажмите **Add Files** и Vivado откроет директорию с нашими HDL-файлами.

    В окне проводника измените *Files of type* на **All Files**. Перейдите в папку `\doc` и выберите недавно скопированный файл `PWM_IP_Core_Datasheet.pdf` и нажмите **OK**. Убедитесь что галочка *Copy Sources into IP Directory* не отмечена. Нажмите OK. Файл будет добавлен в раздел **Product Guide**, а предупреждение пропадет. 

9. Переключимся на окно *Customization Parameters*. Опять, изменения были обнаружены *Vivado* и могут быть объединены. Нажмите **Merge changes...**

    ![Параметры кастомизации](./resources/lab7/IP%20Customization%20Parameters.png)

    Наш параметр **PWM_PERIOD** Появился в папке *Hidden Parameters*.

10. Нажмите правой кнопкой на **PWM_PERIOD**, нажмите **Edit Parameter..** и отметьте галочкой **Visible in Customization GUI**. Нажмите **OK**.

    ![Изменить параметры IP](./resources/lab7/Edit%20IP%20Paramter%20Window.png)

    Это позволит увидеть параметр не в папке *Hidden Parameters*, а в *Customization Parameters*

    ![Добавили параметр в нужную папку](./resources/lab7/User%20Parameters%20Added%20to%20Customization%20Folder.png)

    До версии Vivado 2025.2 присутствовать предупреждение `[IP_Flow 19-11889] HDL Parameter 'C_S00_AXI_DATA_WIDTH (C S00 AXI DATA WIDTH)': Order is obsolete with XGUI version >= 2.0` - его можно проигнорировать (Официальный ответ с форума Xilinx). 

11. Далее выберите вкладку *Ports and Interfaces*. Все пользовательские порты должны быть в этом списке, как показано ниже.

    ![Пользовательские порты](./resources/lab7/User%20IO%20Ports.png)

12. Нажмите правой кнопкой мыши на **Interrupt_Out -> Auto  Infer  Single  Bit  Interface -> Interrupt**, чтобы определить выход как *Interrupt*.

    ![Определение прерывания](./resources/lab7/Defining%20Interrupt.png)

<!-- 13. Раскройте **Clocks and Reset Signals->S00_AXI_CLK**. Нажмите правой кнопкой мыши на **S00_AXI_CLK** и выберите **Edit Interface...**

    ![Починка ошибки FREQ_HZ](./resources/lab7/AXI%20warning%20resolve.png)

    Во вкладке *Parameters* выберите пункт **Requires User Settings**. В нем выберите **FREQ_HZ**. Нажмите на стрелочку для переноса параметра. После добавления параметра нажмите **ОК**

    ![Добавление параметра](./resources/lab7/Adding%20parameter.png) -->

14. Перейдем к **Addressing and Memory**.

    ![Карта памяти](./resources/lab7/Addressing%20and%20Memory%20Map.png)

    В нашем IP нет ни памяти ни *BRAM*. Это окно демонстрирует карту памяти для слейв интерфейса AXI.

15. Далее выберите вкладку *Customization GUI*. Вы можете свободно перемещать параметры в этом окне. Можете например передвинуть параметр PWM Period.

    ![Кастомизация GUI](./resources/lab7/Customization%20GUI.png)

16. Выберите вкладку *Review and Package*.

17. Просмотрите настройки и нажмите кнопку **Package IP**, если вы редактируете ваш IP, то вы увидите кнопку **Re-package IP**.

    ![Упаковка IP](./resources/lab7/Repackage%20IP.png)

22. По окончанию *Vivado* предложит закрыть проект. Нажмите **Yes**. В этот момент временный проект, который создавался в папке `U:\ZynqLabs\ZynqDesign\ZynqDesign.tmp` будет удален. 

## Эксперимент 4: Упаковка IP-проекта

Этот эксперимент покажет, как упаковать IP для IP-каталога.

---

### **Обобщенная инструкция:**

Упакуйте IP.

---

### **Пошаговая инструкция:**

1. Если вы закрыли проект **ZynqDesign**, откройте его заново. 

2. Откройте Block Design.

    ![Открыть Block Design](./resources/lab7/Open%20Block%20Design.png)

3. Выберите значок **Add IP** или щелкните правой кнопкой мыши в окне диаграммы и выберите **Add IP**. Введите поиске PWM и двойным щелчком по найденному IP блоку добавьте его в ваш дизайн. 

    ![Добавить PWM в Design](./resources/lab7/Add%20PWM%20IP%20in%20Design.png)

4. После добавления блока на дизайн должна появится зеленая кнопка **Run Connection Automation.**. Нажмите на нее. 

    ![Выбор Run Connection Automation](./resources/lab7/IP%20Core%20Inserted.png)

5. Убедитесь, что флажок **All Automation** установлен, затем нажмите кнопку **ОК**.

    ![Запуск All Automation](./resources/lab7/Run%20Connection%20Automation%20for%20PWM.png)

    Автоматика корректно настраивает блок AXI SmartConnect для работы в качестве второго ведущего устройства. Затем подключаются AXI, тактовый генератор и устройство сброса, как показано ниже.

    ![После перенастройки AXI SmartConnect](./resources/lab7/PWM%20Controller%20AXI%20clk%20%20and%20reset%20Connected.png)
    
6. У IP-ядра PWM_w_Int есть выход прерывания, но Zynq PS пока не поддерживает прием прерываний PL. Дважды щелкните по Zynq PS.

7. Выберите **Interrupts** в навигаторе страниц.

8. Установите флажок **Fabric Interrupts**, затем разверните его.

9. Разверните раздел **PL-PS Interrupt Ports** и установите флажок рядом с IRQ_F2P[15:0]. F2P — это соединение между ПЛИС и PS. После завершения нажмите **«ОК»**.

    ![Добавление прерываний](./resources/lab7/Add%20PL-PS%20Interrupts.png)

10. Подключите выход Interrupt_Out от PWM IP к IRQ_F2P[0:0] на Zynq PS, щелкнув по любому из контактов, создав линию, а затем щелкнув по другому контакту.

    ![Подключение прерываний](./resources/lab7/Connected%20Interrupt.png)

Теперь добавим ядро интегрированного логического анализатора (ILA) для отладки и наблюдения состояний сигналов после загрузки на ПЛИС. 
    
11. Снова зайдите в раздел «Добавить IP» и найдите ILA:

    ![Добавление ILA](./resources/lab7/Add%20ILA.png)

    Добавьте его в дизайн двойным щелчком.

12. Сделайте двойной щелчок по **ILA**, чтобы открыть его настройки для редактирования. 

13. Установите Monitor Type на **Native** и задайте количество датчиков равным 4. Затем щелкните вкладку **Probe_Ports**.

    ![Настройка ILA](./resources/lab7/Customize%20ILA%20IP.png)

14. Измените значение Probes Widths следующим образом и нажмите **ОК**:

    * PROBE0 = 8
    * PROBE1 = 20
    * PROBE2 = 32
    * PROBE3 = 1

    ![Настройка ILA](./resources/lab7/ILA%20Probes%20Configuration.png)

15. Подключите LED к PROBE0, PWM_Counter — к PROBE1, DutyCycle — к PROBE2, а Interrupt_out — к PROBE3. Подключите clk к FCLK_CLK0.

16. Для вывода сигнала LED на светодиоды нам надо добавить новый порт. Для этого нажмите правой кнопкой по свободному пространству на дизайне и выберете **Create port...** или используйте сочетание клавиш **Ctrl + K**

17. В открывшимся окне введите имя порта **"LED"** и задайте направление **output**. Так же задайте разрядность порта - от 7 до 0. 

    ![Добавление порта](./resources/lab7/Create%20Port.png)

18. Соедините добавленный порт с выходом LED PWM IP.

19. После того, как мы добавили новое IP-ядро, нам нужно обновить карту адресов. Откройте вкладку « Address Editor» в блоке дизайна. Если адрес ещё не назначен, нажмите **Auto Assign Address**.

    ![Редактор адресов](./resources/lab7/Address%20Editor.png)

    На этом подключение нашего IP-ядра к проекту завершено. Мы создали совершенно новый IP-блок, упаковали его в IP Packager, импортировали в проект и подключили к микропроцессорной системе Zynq. В следующей лабораторной работе мы протестируем это IP-ядро через интерфейс отладки, а затем добавим приложение для взаимодействия с ним.

    На рисунке приведен полностью собранный дизайн:

    ![Готовый дизайн](./resources/lab7/Fully%20Connected%20Design.png)

## Эксперимент 5: Добавить ядро ​​LogiCORE™ IP JTAG-AXI

Прежде чем продолжить, добавим ещё один компонент, который будет использоваться в следующей лабораторной работе. Этот эксперимент показывает, как добавить ядро ​​JTAG-AXI. Мы объясним этот IP-адрес в следующем разделе, но проще добавить его сейчас, так как в конце этой лабораторной работы мы сгенерируем битовый поток.

---

### **Обобщенная инструкция:**

Добавьте JTAG в AXI Master IP в проект и подключите его к конструкции.

---

### **Пошаговая инструкция:**

1. В Block Design выберите **Add IP**, затем добавьте *JTAG to AXI Master Core* (найдите JTAG).

    ![Add JTAG to AXI Master](./resources/lab7/Add%20JTAG%20to%20AXI%20Master.png)

    Для подключения к ведущему устройству JTAG/AXI требуется ведомый интерфейс. Обратите внимание, что это ведущее устройство, поэтому оно будет генерировать команды и/или данные для передачи ведомому устройству. Подключив его непосредственно к AXI SmartConnect, ведущее устройство JTAG/AXI сможет взаимодействовать со всеми периферийными устройствами, подключенными к этому межсоединению.

2. Нажмите **Run Connection Automation**. Убедитесь, что галочка **All Automation** выбрана и нажмите **"ОК"**

    ![Run Connection Automation on JTAG AXI](./resources/lab7/Run%20Connection%20Automation%20on%20JTAG%20AXI.png)

    Окончательный дизайн блока должен выглядеть следующим образом:

    ![Final Design](./resources/lab7/Final%20Design.png)

3. Выполните валидацию дизайна. Если все хорошо, сохраните его.

4. Щелкните правой кнопкой мыши по **Z_system_i** во вкладке Sources проекта и выберите **Reset Output Products**, а затем **Generate Output Products**.

5. Для подключения светодиодов необходимо добавить файл ограничений, в котором будут указаны используемые пины. Для этого создайте новый файл ограничений, используя пункт **Add Source** на панели **Flow Navigator**. В качестве типа создаваемого файла укажите **"Add or create constraints"**

    ![Add Constraints](./resources/lab7/Add%20Constraints.png)

6. Создайте новый файл ограничений с именем top и нажмите **Finish**. 

    ![Add new constraints](./resources/lab7/Create%20Constraints.png)

7. Откройте созданный файл во вкладке Source в разделе Constraints.

    ![Open XDC](./resources/lab7/Open%20XDC.png)

8. Добавьте в файл следующие строки:

    ```tcl
    set_property PACKAGE_PIN T22 [get_ports {LED0}];
    set_property PACKAGE_PIN T21 [get_ports {LED1}];
    set_property PACKAGE_PIN U22 [get_ports {LED2}];
    set_property PACKAGE_PIN U21 [get_ports {LED3}];
    set_property PACKAGE_PIN V22 [get_ports {LED4}];
    set_property PACKAGE_PIN W22 [get_ports {LED5}];
    set_property PACKAGE_PIN U19 [get_ports {LED6}];
    set_property PACKAGE_PIN U14 [get_ports {LED7}];

    set_property IOSTANDARD LVCMOS33 [get_ports -of_objects [get_iobanks 33]];
    ```

    Команды `set_property PACKAGE_PIN`  задают пины для подключения соответствующих сигналов, а `set_property IOSTANDARD` определяет напряжение на пинах. Сохраните файл после изменения.

8. На панели **Flow Navigator** нажмите **Generate Bitstream**. Это займёт несколько минут в зависимости от вашего компьютера. 

### **Вопросы:**

* Где расположен проект IP-ядра?


## Дальнейшее изучение

При наличии свободного времени можно также изучить:

* Исследуйте предоставленный C-код. Исследуйте конфигурацию PS DMA.

## Источники:

https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/microzed/microzed-board-family (только с VPN)

https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/picozed/picozed-board-family (только с VPN)

https://www.avnet.com/wps/portal/us/products/avnet-boards/avnet-board-families/zedboard/zedboard-board-family (только с VPN)

https://www.xilinx.com/products/silicon-devices/soc/zynq-7000.html

https://www.xilinx.com/products/design-tools/vitis.html

https://www.xilinx.com/developer/products/vivado.html

https://docs.xilinx.com/r/en-US/ug949-vivado-design-methodology/Introduction

https://docs.xilinx.com/v/u/en-US/ug1046-ultrafast-design-methodology-guide

## Ответы

> Был ли необходим новый *Platform Project* для этого приложения?

Platform Project необходимо генерировать заново каждый раз когда мы вносим изменения в аппаратную платформу. Мы назвали ее одинаково, однако она является новой.

> Какого улучшение производительности получается при передачи 8192 байт данных из BRAM в DDR3? Попробуйте с DDR3-DDR3? BRAM-BRAM?

11x, 3x, 20x (результаты могут варьироваться)

> Какого улучшение производительности при передачи 256 байт данных из BRAM в DDR3? Попробуйте с DDR3-DDR3? BRAM-BRAM?

10x, 1x, 18x (результаты могут варьироваться)


5. Если у вас появилось предупреждение `[IP_Flow 19-11770] Clock interface 'S00_AXI_CLK' has no FREQ_HZ parameter.` Выполните следующие шаги:

    1. Выберите поле *Ports and Interfaces*, которое помечено предупреждающим знаком. Далее 