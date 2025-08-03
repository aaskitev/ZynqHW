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

4. Нажмите **Next >** В приветственном окне.

5. Мы будем создавать новую AXI-периферию, поэтому отметьте пункт **Create a New AXI4 peripheral**. Нажмите **Next >**.

6. Введите в соответствующие поля следующую информацию:
    * Name: `PWM_w_Int`
    * Version: `1.0`
    * Display name: `PWM_w_Int_v1.0`
    * Description: `PWM with Interrupt option`
    * IP location: `C:/ZynqHW/2023.1/ip_repo`

    Нажмите **Next >**.

    ![Информация об IP](./resources/lab7/IP%20Details.png)

7. Наша периферия достаточно проста и не требует большой пропускной способности шины, поэтому нам достаточно интерфейс AXI-Lite. Наше устройство также будет слейвом относительно *PS*, который будет мастером. Нам достаточно иметь 32 битный интерфейс. Для наших целей нам достаточно одного регистра, однако 4 - их минимальное количество. Таким образом, нам достаточно параметров **по-умолчанию**. Нажмите **Next >**.

    ![Настройки интерфейса IP](./resources/lab7/IP%20Interface%20Settings.png)

8. Выберите **Add IP to the repository**. Проверьте путь до каталога. Нажмите **Finish**.

    ![Итоги создания IP](./resources/lab7/IP%20Creation%20Summary.png)

### **Вопросы:**

* Где расположен проект IP-ядра?

## Эксперимент 2: Кастомизация нового IP-проекта

Этот эксперимент покажет, как добавить кастомную пользовательскую логику и инстанцировать ее в IP-проект. Также мы кастомизируем наш IP-проект, добавив в него новый порт и параметры.

![Схема Дизайна](./resources/lab7/Design%20Scheme.png)

---

### **Обобщенная инструкция:**

Импортируйте пользовательский код описания аппаратуры **PWM_Controller_Int.v** и инстанцируйте ее в проект. Соедините с **slv_reg0** из интерфейса AXI. соедините выходы *PWM* с модулем верхнего уровня.

---

### **Пошаговая инструкция:**

1. В *Vivado* выберите **Window->IP Catalog**.

2. Выберите IP в каталоге IP, введя в поиске `PWM`. Нажмите правой кнопкой мыши и выберите **Edit in IP Packager**.

    ![Отредактировать IP](./resources/lab7/Edit%20IP%20Project.png)

3. Мы принимаем настройки имени и расположения проекта по-умолчанию и нажимаем **OK**.

    ![Имя и расположения проекта IP](./resources/lab7/IP%20Project%20Name%20and%20Location.png)

    По завершению откроется новое окно *Vivado Project*. В этом новом окне мы будем редактировать на IP. Обратите внимание на корневой каталог (Root Directory) нашего проекта и его имя (Project Name). Это будет важно при дальнейшем выполнении работы. \<IP Name\>_project.xpr - проект, который будет открыт для редактирования IP. В нашем случае наш проект IP назван `PWM_w_Int_v1_0_project.xpr`.

4. В окне *Sources* раскройте пункт **Design Sources**. Обратите внимание на то, что, когда мы в окне с исходниками выбираем файл, его свойства открываются в окне *Source File Properties*. Это полезно для определения того, где расположены файлы с HDL на вашем компьютере.

    ![Исходники и их свойства](./resources/lab7/Design%20Source%20Properties.png)

5. Если у вас появилось предупреждение `[IP_Flow 19-11770] Clock interface 'S00_AXI_CLK' has no FREQ_HZ parameter.` Выполните следующие шаги:

    1. Выберите поле *Ports and Interfaces*, которое помечено предупреждающим знаком. Далее раскройте **Clocks and Reset Signals->S00_AXI_CLK**. Нажмите правой кнопкой мыши на **S00_AXI_CLK** и выберите **Edit Interface...**

        ![Починка ошибки FREQ_HZ](./resources/lab7/AXI%20warning%20resolve.png)

    2. Во вкладке *Parameters* выберите пункт **Requires User Settings**. В нем выберите **FREQ_HZ**. Нажмите на стрелочку для переноса параметра.

    ![Добавление параметра](./resources/lab7/Adding%20parameter.png)

6. Вернемся к нашим исходникам. Мы видим два VHDL файла. Первый `PWM_w_Int_v1_0.vhd` - верхнеуровневый модуль-обертка. Откройте этот файл дважды **нажав** на него. В районе 84 строчки он инстанцирует интерфейс AXI `PWM_w_Int_v1_0_S00_AXI`, который определен в файле `PWM_w_Int_v1_0_S00_AXI.vhd`. Далее находится место для добавления пользовательской логики, куда мы и добавим логику нашего IP.

    ![Расположение пользовательской логики](./resources/lab7/User%20Logic%20Location.png)

7. Теперь скопируйте **PWM_Controller.v** из `2023.1/Support_documents` в `C:\ZynqHW\2023.1\ip_repo\PWM_w_Int_1_0\hdl`

8. В области *Flow Navigator* выберите **Add Sources**. Далее выберите **Create Design Sources**. После завершения нажмите **Next>**.

    ![Добавление исходника](./resources/lab7/Add%20Design%20Sources.png)

9. В следующем окне выберите **Add Files**.

10. Измените директорию на `C:\ZynqHW\2023.1\ip_repo\PWM_w_Int_1_0\hdl`. Нажмите дважды на **PWM_Controller_Int.v**, чтобы добавить его в проект.

11. Проверьте файл. Также удостоверьтесь, что *Copy Sources into IP Directory* не отмечено галочкой. Нажмите **Finish**.

    ![Добавление файла в проект](./resources/lab7/Add%20Files%20to%20Project.png)

12. Теперь файл появится в нашем окне с исходниками. Но не внутри нашего файла верхнего уровня. Это ожидаемо, поскольку мы не инстанцировали этот код в файл верхнего уровня.

    ![Новый исходник в проекте](./resources/lab7/New%20Source%20in%20Project.png)

13. Откройте **PWM_Controller_Int**, дважды нажав на него. Изучите файл и инстанцируйте этот модуль в **PWM_w_Int_v1_0.vhd**. Дважды нажмите на **PWM_w_Int_v1_0.vhd**, объявите компонент PWM_Controller_Int в секции объявления компонентов файла **PWM_w_Int_v1_0.vhd** в районе 52 строчки:

    ```VHDL
	    -- component declaration
        component PWM_Controller_Int is
	        generic (
	        period : integer := 20
	        );
	        port (
	        Clk : in std_logic;
	        DutyCycle : in std_logic_vector(31 downto 0);
	        Reset : in std_logic;
	        PWM_out : out std_logic_vector(7 downto 0);
	        Interrupt : out std_logic;
	        count : out std_logic_vector(period - 1 downto 0)
	        );
	    end component PWM_Controller_Int;
    ```

14. Внизу файла **PWM_w_Int_v1_0.vhd** Добавьте следующий код в секцию пользовательской логики в районе 130 строчки:

    ```VHDL
    	-- Add user logic here
    PWM_Controller_Int_Inst : PWM_Controller_Int
        generic map (
            period =>
        )
        port map (
            Clk =>,
            DutyCycle =>,
            Reset =>,
            PWM_out =>,
            Interrupt =>,
            count =>
        );
    	-- User logic ends
    ```

    Нажмите сохранить. Это нормально, что текущий файл показывает синтаксические ошибки, входы и выходы мы соединим позже.

15. Как только мы ввели этот код, наш файл с кодом логики IP будет внутри верхнеуровнего файла

    ![Пользовательская логика тепеь в иерархии проекта](./resources/lab7/User%20Logic%20now%20in%20Project%20Hierarchy.png)

    Следующие шаги покажут методологию соединения пользовательской логики с нашим IP.

16. Сейчас нам нужно соединить входы и выходы нашей пользовательской логики. Начните с очевидных сигналов модуля пользовательской логики `PWM_Controller_Int`. Тактовые сигналы и сигналы сброса имеют прямую ассоциацию между сигналами контроллера и Slave AXI.
    
    ```VHDL
    ...
    Clk => s00_axi_aclk,
    ...
    Reset => s00_axi_aresetn,
    ...
    ```

    Теперь соединим выходы. Эти сигналы должны быть объявлены в списке портов модуля верхнего уровня. У этого IP будет 4 выхода.
        
    * **LEDs**: выходы *PWM*, соединенные с LED'ами.
    * **Interrupt_out**: сигнал прерывания, показывающий о некорректных настройках *PWM*.
    * **PWM_Counter**: (для отладки) работающий в свободном режиме счетчик *PWM*.
    * **DutyCycle**: (Для отладки) Значение переданное с *PS* через интерфейс *AXI* для контролирования скважности *PWM*.
    * Добавим также настраиваемый параметр **PWM_PERIOD**, который устанавливает глубину счетчика *PWM*.

17. Добавим следующие порты в самом верху файла с кодом (около 5 строчки) и пользовательскую HDL логику:

    ```VHDL
    entity PWM_w_Int_v1_0 is
    	generic (
    		-- Users to add parameters here
            PWM_PERIOD : integer := 20;
    		-- User parameters ends
    		-- Do not modify the parameters beyond this line


    		-- Parameters of Axi Slave Bus Interface S00_AXI
    		C_S00_AXI_DATA_WIDTH	: integer	:= 32;
    		C_S00_AXI_ADDR_WIDTH	: integer	:= 4
    	);
    	port (
    		-- Users to add ports here
            LEDs : out std_logic_vector(7 downto 0);
            Interrupt_out : out std_logic;
            PWM_Counter : out std_logic_vector(PWM_PERIOD - 1 downto 0);
            DutyCycle : out std_logic_vector(31 downto 0);
    		-- User ports ends
    ```

    Добавим сигнал в районе 98-ой строчки:

    ```VHDL
    	end component  PWM_w_Int_v1_0_S00_AXI;
    signal DutyCycle_int : std_logic_vector(31 downto 0);

    begin

    DutyCycle <= DutyCycle_int;
    -- Instantiation of Axi Bus Interface S00_AXI
    ```

    Полная пользовательская логика в районе 135-ой строчки:

    ```VHDL
    	-- Add user logic here
    PWM_Controller_Int_Inst : PWM_Controller_Int
        generic map (
            period => PWM_PERIOD
        )
        port map (
            Clk => s00_axi_aclk,
            DutyCycle => DutyCycle_int,
            Reset => s00_axi_aresetn,
            PWM_out => LEDs,
            Interrupt => Interrupt_Out,
            count => PWM_Counter
        );
    	-- User logic ends
    ```

    Таким образом, мы полностью подсоединили наш IP. Однако мы еще не соединили источник *DutyCycle*. Этот сигнал идет от процессора. К нашему удобству, для этих целей во время запуска *Create IP Peripheral wizard* Vivado создал для нас *AXI proxy*.

18. Откройте **PWM_w_Int_v1_0_S00_AXI.vhd**. Просмотрите файл на строчках 100-197. Здесь вы увидите объяснения и определения для четырех регистров, созданных *Vivado*. Нам нужен только один из них - **slv_reg0**.

19. Сделаем *slv_reg0* выходом нашего модуля. В районе 19-ой строчки добавьте выход (**slave_reg0**, к примеру) в области для пользовательских портов. Затем соедините его с slv_reg0 (в районе 123 строчки):

    ```VHDL
    	port (
		    -- Users to add ports here
            slave_reg0 : out std_logic_vector(C_S_AXI_DATA_WIDTH - 1 downto 0);
		    -- User ports ends
    ```

    ```VHDL
    begin
	    -- I/O Connections assignments
        slave_reg0      <= slv_reg0;
    ```

20. **Сохраните** файл, нажав на **Ctrl-S**, и вернитесь обратно к файлу модуля верхнего уровня иерархии *PWM_w_Int_v1_0.vhd*.

    Так как мы добавили новый порт к нашему слейву AXI, мы должны соединить его с модулем верхнего уровня. Напомним, что этот регистр содержит значение **DutyCycle**, переданное от процессора.

21. Добавьте порт *slave_reg0* к определению компонента слейв AXI-интерфейса (в районе 76 строчки):

    ```VHDL
            port (
		    slave_reg0      : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		    S_AXI_ACLK	: in std_logic;
    ```

    Соедините его с сигналом **DutyCycle_int** в районе 112 строчки. Сохраните файл.

    ```VHDL
    PWM_w_Int_v1_0_S00_AXI_inst : PWM_w_Int_v1_0_S00_AXI
	    generic map (
	    	C_S_AXI_DATA_WIDTH	=> C_S00_AXI_DATA_WIDTH,
	    	C_S_AXI_ADDR_WIDTH	=> C_S00_AXI_ADDR_WIDTH
	    )
	    port map (
	        slave_reg0      => DutyCycle_int,
	    	S_AXI_ACLK	=> s00_axi_aclk,
    ```

22. На этом мы закончили проводить все соединения. Выберите файл модуля верхнего уровня *PWM_w_Int_v1_0.vhd* и нажмите **Run Synthesis**, для проверки дизайна.

23. Не запускайте имплементацию. Откройте ссинтезированный дизайн из диалогового окна *Synthesis Completed*.

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

3. В следующем окне *Compatibility* мы можем выбрать семейства устройств, совместимых с нашим IP. *Zynq* включен сюда по-умолчанию, как целевое устройство, которое использовалось при создании IP. Больше устройств может быть добавлено, нажав на плюсик. Измените *Life Cycle* на **Production**.

    ![Совместимость IP](./resources/lab7/IP%20Compatibility.png)

4. Следующее окно - *File Groups*. Обратите внимание на уведомление вверху окна. Это говорит о том, что *Vivado* заметил изменения, которые мы сделали в HDL. Нажмите на **Merge changes from File Groups Wizard**.

    ![Файлы IP](./resources/lab7/IP%20File%20Groups.png)

5. После того как наши изменения были смерджены, раскройте все списки файлов. Заметьте, что средство слияния файлов добавило наш пользовательский HDL *PWM_Controller_Int*. Однако этот модуль, написанный на *Verilog* находится в группе с VHDL, что вызывает критическое предупреждение.

    ![Группы файлов](./resources/lab7/File%20Groups.png)

6. Выберите **Standard**, нажмите правой кнопкой мыши и выберите **Add File Group...** Для использования и VHDL- и Verilog-модулей выберите группы **Synthesis** и **Simulation** и нажмите **OK**.

    ![Добавим группы файлов](./resources/lab7/Add%20File%20Group.png)

7. Теперь кликните правой кнопкой мыши по **Synthesis** и выберите **Add Files**.

8. Когда откроется окно добавления файлов, нажмите **Add Files** и Vivado откроет директорию с нашими HDL-файлами.

    В окне проводника измените *Files of type* на **All Files**. Выберите **PWM_Controller_Int.v**, **PWM_w_Int_v1_0_S00_AXI.vhd** и **PWM_w_Int_v1_0.vhd** и нажмите **OK**. Убедитесь что галочка *Copy Sources into IP Directory* не отмечена. Нажмите OK.

    ![Добавим файлы в группы](./resources/lab7/Add%20HDL%20Files%20to%20IP%20File%20Groups.png)

9. Повторите процесс для группы *Simulation*.

10. Теперь у нас 2 ошибки, 2 критических предупреждения

    ![Две ошибки и предупреждения](./resources/lab7/Two%20Errors%20and%20warnings.png)

11. Определите **PWM_w_Int_v1_0** в поле *Model Name* для групп *Synthesis* и *Simulation* в папке *Standard*. Файловые группы должны выглядеть следующим образом и ошибки пропадут:

    ![Ошибки исчезли](./resources/lab7/Error%20Dissapear.png)

12. Переключимся на окно *Customization Parameters*. Опять, изменения были обнаружены *Vivado* и могут быть смерджены. Нажмите **Merge changes...**

    ![Параметры кастомизации](./resources/lab7/IP%20Customization%20Parameters.png)

    Наш параметр **PWM_PERIOD** Появился в папке *Hidden Parameters*.

13. Вернемся обратно к *File Groups* и удалим группы **VHDL Synthesis** и **VHDL Simulation**. Все предупреждения должны исчезнуть.

    ![Убираем группы](./resources/lab7/Remove%20VHDL%20Synthesis%20and%20Simulation%20Groups.png)

14. Вернитесь к *Customization Parameters* и правой кнопкой нажмите на **PWM_PERIOD**, нажмите **Edit Parameter..** и отметьте галочкой **Visible in Customization GUI**. Нажмите **OK**.

    ![Изменить параметры IP](./resources/lab7/Edit%20IP%20Paramter%20Window.png)

    Это позволит увидеть параметр не в папке *Hidden Parameters*, а в *Customization Parameters*

    ![Добавили параметр в нужную папку](./resources/lab7/User%20Parameters%20Added%20to%20Customization%20Folder.png)

15. Далее выберите вкладку *Ports and Interfaces*. Все пользовательские порты должны быть в этом списке, как показано ниже.

    ![Пользовательские порты](./resources/lab7/User%20IO%20Ports.png)

16. Нажмите правой кнопкой мыши на **Interrupt_Out -> Auto  Infer  Single  Bit  Interface -> Interrupt**, чтобы определить выход как *Interrupt*.

    ![Определение прерывания](./resources/lab7/Defining%20Interrupt.png)

17. Перейдем к **Addressing and Memory**.

    ![Карта памяти](./resources/lab7/Addressing%20and%20Memory%20Map.png)

    В нашем IP нет ни памяти ни *BRAM*. Это окно демонстрирует карту памяти для слейв интерфейса AXI.

18. Далее выберите вкладку *Customization GUI*.

    ![GUI превью](./resources/lab7/IP%20GUI%20Preview.png)

19. Вы можете свободно перемещать параметры в этом окне. Можете например передвинуть параметр PWM Period.

    ![Кастомизация GUI](./resources/lab7/Customization%20GUI.png)

20. Выберите вкладку *Review and Package*.

21. Просмотрите настройки и нажмите кнопку **Package IP**, если вы редактируете ваш IP, то вы увидите кнопку **Re-package IP**.

    ![Упаковка IP](./resources/lab7/Repackage%20IP.png)

22. По окончанию *Vivado* предложит закрыть проект. Нажмите **Yes**.

## Эксперимент 4: Упаковка IP-проекта

Этот эксперимент покажет, как упаковать IP для IP-каталога.

---

### **Обобщенная инструкция:**

Упакуйте IP.

---

### **Пошаговая инструкция:**

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