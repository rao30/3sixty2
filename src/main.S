#Directives
  		.syntax unified
  		.cpu cortex-m0
  		.fpu softvfp
	  	.thumb

#Equates
		.equ	NVIC, 0xE000E100
		.equ 	ISER, 0x0
		.equ	ICER, 0x80
		.equ	ISPR, 0x100
		.equ	ICPR, 0x180
		.equ	IPR0, 0x300
		.equ	IPR1, 0x304
		.equ	IPR2, 0x308
		.equ	IPR3, 0x30C
		.equ	IPR4, 0x310
		.equ	IPR5, 0x314
		.equ	IPR6, 0x318
		.equ	IPR7, 0x31C

		.equ	RCC, 0x40021000
		.equ	AHBENR, 0x14
		.equ	APB1ENR, 0x1C

		.equ	TIM2, 0x40000000
		.equ	CR1, 0x00
		.equ	CR2, 0x04
		.equ	SMCR, 0x08
		.equ	DIER, 0x0C
		.equ	SR, 0x10
		.equ	EGR, 0x14
		.equ	CCMR1, 0x18
		.equ	CCMR2, 0x1C
		.equ	CCER, 0x20
		.equ	CNT, 0x24
		.equ	PSC, 0x28
		.equ	ARR, 0x2C
		.equ	CCR1, 0x34
		.equ	CCR2, 0x38
		.equ	CCR3, 0x3C
		.equ	CCR4, 0x40
		.equ	DCR, 0x48
		.equ	DMAR, 0x4C

		.equ	GPIOA,	0x48000000
		.equ	GPIOB,	0x48000400
		.equ	GPIOC,	0x48000800
		.equ	GPIOD,	0x48000C00
		.equ	GPIOF,  0x48001400
		.equ	MODER,		0x00
		.equ	OTYPER,		0x04
		.equ	OSPEEDR,	0x08
		.equ	PUPDR,	 	0x0C
		.equ	IDR,	 	0x10
		.equ	ODR,	 	0x14
		.equ	BSRR,	 	0x18
		.equ	LCKR,	 	0x1C
		.equ	AFRL,	 	0x20
		.equ	AFRH,	 	0x24
		.equ	BRR,	 	0x28
		.equ	enA,	0x00020000
		.equ	enC,	0x00080000
		.equ	outBlue,0x00010000
		.equ	outBG,	0x00050000
		.equ	halfway, 100
		.equ	onesec, 20
		.equ	finish, 200
		.equ	lBound, 90
		.equ	uBound,  110
.global	main
.global TIM2_ISR

main:
 		nop
gpioConfig:
		#perform GPIO configurations here
		#Activate GPIOC Clock
				ldr r5, = RCC
				ldr r6, = AHBENR
				ldr r0, [r5,r6]
				ldr r1, =enC
				orrs r0,r0,r1
				str r0, [r5,r6]
				#Configure PC8 as output
				ldr r5, =GPIOC
				ldr r6, =MODER
				ldr r0, [r5,r6]
				ldr r1, =outBG
				orrs r0, r0, r1
				str r0, [r5,r6]
				#Activate GPIOB Clock
				ldr r5, = RCC
				ldr r6, = AHBENR
				ldr r0, [r5,r6]
				ldr r1, = enA
				orrs r0,r0,r1
				str r0, [r5,r6]
				#Configure PA0 as input
				ldr r5, =GPIOA
				ldr r6, =MODER
				ldr r0, [r5,r6]
				ldr r1, =0x00000000
				orrs r0, r0, r1
				str r0, [r5,r6]
tim2Config:
				ldr r5, =RCC
				ldr r6, =APB1ENR
				ldr r0, [r5,r6]
				ldr r1, =0x0001
				orrs r0,r0,r1
				str r0, [r5, r6]

				ldr r5, =TIM2
				ldr r6, =PSC
				ldr r0, [r5,r6]
				ldr r1, =50430
				str r1, [r5, r6]

				ldr r5, =TIM2
				ldr r6, =ARR
				ldr r0, [r5,r6]
				ldr r1, = 7
				str r1, [r5, r6]

				ldr r5, =TIM2
				ldr r6, =DIER
				ldr r0, [r5,r6]
				ldr r1, =0x0001
				str r1, [r5, r6]

				ldr r5, =TIM2
				ldr r6, =CR1
				ldr r0, [r5,r6]
				ldr r1, = 0x0001
				orrs r0, r0, r1
				str r0, [r5, r6]


nvicConfig:
				ldr r5, =NVIC
				ldr r6, =ISER
				ldr r0, [r5,r6]
				ldr r1, =0x00008000
				orrs r0, r1
				str r0, [r5, r6]
				ldr r5, =NVIC
				ldr r6, =ICPR
				ldr r0, [r5,r6]
				ldr r1, =0x00008000
				orrs r0, r1
				str r0, [r5, r6]




#6.3 led toggle
/*infiniteLoop:
		ldr r0, =btnPress
		ldr r1, [r0]
		cmp r1, #1
		beq LedToggle
		b infiniteLoop

LedToggle:
		//Reset btnPress to 0
		ldr r1, = 0x0
		str r1, [r0, #0]
		//Toggle the LED
		ldr r5, =GPIOC
		ldr r6, =ODR
		ldr r0, [r5, r6]
		ldr r1, = 0x00000200
		ands r0, r1
		cmp r0, #0
		beq LedOn
		b LedOff
LedOff: ldr r1, = 0x0
		str r1, [r5,r6]
		b infiniteLoop
LedOn: str r1, [r5,r6]
		b infiniteLoop */
infiniteLoop: ldr r0, =btnPress
				ldr r1, [r0]
				cmp r1, #1
				beq startCounter
				b infiniteLoop

startCounter:	ldr r1, = 0x0
				str r1, [r0, #0]
				ldr r0, =counterFlag
				ldr r1, = 0x0001
				str r1, [r0, #0]


reactionLoop:
			ldr r0, =btnPress
			ldr r1, [r0]
			cmp r1, #1
			beq timeCheck
			b timeoutCheck

timeCheck:	ldr r1, = 0x0
			str r1, [r0, #0]
			ldr r0, =lBound
			ldr r1, =counter
			ldr r2, [r1]
			cmp r2, r0
			blt defeat
			bge timeCheckUpper
timeCheckUpper:	ldr r0, =uBound
				ldr r1, =counter
				ldr r2, [r1]
				cmp r2, r0
				ble victory
				bgt defeat

timeoutCheck:
			ldr r1, =finish
			ldr r0, =counter
			ldr r2, [r0]
			cmp r2, r1
			bge defeat
			b reactionLoop
victory:	ldr r0, =GPIOC
			ldr r1, =ODR
			ldr r2, = 0x00000200
			str r2, [r0, r1]

victoryLoop: 	ldr r0, =btnPress
				ldr r1, [r0]
				cmp r1, #1

				beq resetGame
				b victoryLoop

defeat:		ldr r0, =defeatFlag
			ldr r1, = 0x0001
			str r1, [r0, #0]

defeatLoop:	ldr r0, = defeatCounter
				ldr r1, [r0]
				ldr r2, =onesec
				cmp r1, r2
				beq toggleGreen
				bne defeatLoop

toggleGreen:
				#incrementing defeatToggleCOunter by one and checking for 4
				ldr r0, = defeatToggleCounter
				ldr r1, [r0]
				adds r1, #1
				str r1, [r0, #0]
				#Resetting defeatCounter
				ldr r0, =defeatCounter
				ldr r1, = 0x0
				str r1, [r0, #0]

				#Toggling GPIO
				ldr r0, =GPIOC
				ldr r1, =IDR
				ldr r2, [r0, r1]
				movs r3, r2
				ldr r1, =0x00000200
				ands r2, r1
				ldr r1, =ODR
				cmp r2, #0
				beq turnongreen
				bne turnoffgreen

turnongreen:	ldr r2, =0x00000200
				orrs r2, r3
				str r2, [r0, r1]
				b checkdefeat
turnoffgreen:	ldr r2, =0xFFFFFDFF
				ands r2, r3
				str r2, [r0, r1]
				b checkdefeat


				#Checking defeattogglecounter = 4
checkdefeat:	ldr r0, =defeatToggleCounter
				ldr r1, [r0]
				cmp r1, #4
				bge resetCheck
				blt defeatLoop

resetCheck:		ldr r0, =btnPress
				ldr r1, [r0]
				cmp r1, #1
				beq resetGame
				b resetCheck

resetGame:
			ldr r1, = 0x0
			str r1, [r0, #0]
			ldr r0, =GPIOC
			ldr r1, =ODR
			ldr r2, = 0x0
			str r2, [r0, r1]
			ldr r0, =counter
			str r2, [r0, #0]
			ldr r0, =counterFlag
			str r2, [r0, #0]
			ldr r0, =defeatFlag
			str r2, [r0, #0]
			ldr r0, = defeatToggleCounter
			str r2, [r0, #0]
			ldr r0, =defeatCounter
			str r2, [r0, #0]
			b infiniteLoop
#6.2 Square wave generation
/*TIM2_ISR:
				ldr r5, =NVIC
				ldr r6, =ICPR
				ldr r0, [r5,r6]
				ldr r1, =0x00008000
				orrs r0, r1
				str r0, [r5, r6]

				ldr r5, =TIM2
				ldr r6, =SR
				ldr r0, [r5,r6]
				ldr r1, =0x0
				str r1, [r5, r6]

				ldr r5, =GPIOC
				ldr r6, =ODR
				ldr r0, [r5,r6]
				ldr r1, =0xFFFFFFFF
				eors r0, r1
				str r0, [r5, r6]
				b isrReturn

*/
		#Decrement ms counter

#6.3 Using Timer for button debouncing
/*TIM2_ISR:
				ldr r5, =TIM2
				ldr r6, =SR
				ldr r0, [r5,r6]
				ldr r1, =0x0
				str r1, [r5, r6]
buttonDebounce:
		#Check button press
		ldr r3, = btnPrev
		ldr r6, = btnCurrent
		ldr r0, [r6]
		str r0, [r3,#0]
		ldr r5, = GPIOA
		ldr r6, = IDR
		ldr r0, [r5,r6]
		ldr r2, =0x00000001
		ands r0, r2
		ldr r5, =btnCurrent
		str r0, [r5, #0] //button current
		ldr r1, [r3]  //Button previous
		cmp r1, r0
		bgt buttonPressed
		b isrReturn

buttonPressed:
		ldr r5, = btnPress
		ldr r0, = 0x0001
		str r0, [r5, #0] */

#6.4 Reaction Timer
TIM2_ISR:


buttonDebounce:
				#Check button press
				ldr r3, = btnPrev
				ldr r6, = btnCurrent
				ldr r0, [r6]
				str r0, [r3,#0]
				ldr r5, = GPIOA
				ldr r6, = IDR
				ldr r0, [r5,r6]
				ldr r2, =0x00000001
				ands r0, r2
				ldr r5, =btnCurrent
				str r0, [r5, #0] //button current
				ldr r1, [r3]  //Button previous
				cmp r1, r0
				bgt buttonPressed
				b counterCheck

buttonPressed:
				ldr r5, = btnPress
				ldr r0, = 0x0001
				str r0, [r5, #0]

counterCheck:
				ldr r5, =counterFlag
				ldr r6, [r5]
				cmp r6, #1
				beq increment
				b defeatCheck

increment:
				ldr r5, = counter
				ldr r6, [r5]
				adds r6, 0x0001
				str r6, [r5, #0]
				ldr r0, =halfway
				cmp r6, r0
				beq TurnBlueOn
				bne defeatCheck

				//Turns blue led on halfway thru
TurnBlueOn:		ldr r0, =GPIOC
				ldr r1, =ODR
				ldr r2, [r0, r1]
				ldr r3, = 0x0100
				orrs r2, r3
				str r2, [r0, r1]
				b defeatCheck

defeatCheck:	ldr r0, =defeatFlag
				ldr r1, [r0]
				cmp r1, #1
				beq incrementDefeat
				bne isrReturn

incrementDefeat: ldr r0, = defeatCounter
					ldr r1, [r0]
					adds r1, #1
					str r1, [r0, #0]
					b isrReturn

isrReturn:
				ldr r5, =TIM2
				ldr r6, =SR
				ldr r0, [r5,r6]
				ldr r1, =0x0
				str r1, [r5, r6]
				ldr r5, =NVIC
				ldr r6, =ICPR
				ldr r0, [r5,r6]
				ldr r1, =0x00008000
				orrs r0, r1
				str r0, [r5, r6]
				bx	 lr

		.data
		.align 4
defeatToggleCounter: .space 4
defeatCounter:  .space 4
defeatFlag:    .space 4
counterFlag:    .space 4
counter:		.space 4
btnCurrent:		.space 4
btnPrev:		.space 4
btnPress:		.space 4
msCount:	  .space 4
buttonPress:  .space 4
buttonState0: .space 4
